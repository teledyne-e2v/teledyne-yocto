/*
 *  V4L2 video capture example
 *
 *  This program can be used and distributed without restrictions.
 *
 *      This program is provided with the V4L2 API
 * see http://linuxtv.org/docs.php for more information
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <getopt.h>             /* getopt_long() */

#include <fcntl.h>              /* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <linux/videodev2.h>

#define CLEAR(x) memset(&(x), 0, sizeof(x))

#define V4L2_QUEUE_SIZE 3

#define DEFAULT_FRAME_COUNT 100
#define DEFAULT_WIDTH       1920
#define DEFAULT_HEIGHT      1080
#define DEFAULT_PIX_FMT     V4L2_PIX_FMT_GREY
#define DEFAULT_PATH_1      "output1.raw"
#define DEFAULT_PATH_2      "output2.raw"

/* Configuration for fast resolution switching */
#define ROI_WIDTH       DEFAULT_WIDTH
#define ROI1_HEIGHT     DEFAULT_HEIGHT
#define ROI2_HEIGHT     540
#define ROI1_SELECT     1
#define ROI2_SELECT     2
#define IDLE_ENABLE     1
#define IDLE_DISABLE    0
#define RES_SW_ENABLE   1

/* TOPAZ2M CIDs*/
enum TOPAZ2M_CUSTOM_V4L2_CID {
	V4L2_CID_FRAME_RATE = V4L2_CID_BASE+43,
	V4L2_CID_ROI_OFFSET_COL,
	V4L2_CID_ROI_SELECT,
	V4L2_CID_ROI_WIDTH,
	V4L2_CID_ROI1_ANA_GAIN,
	V4L2_CID_ROI1_DIG_GAIN,
	V4L2_CID_ROI1_EXPOSURE,
	V4L2_CID_ROI1_HEIGHT,
	V4L2_CID_ROI1_HFLIP,
	V4L2_CID_ROI1_VFLIP,
	V4L2_CID_ROI1_OFFSET_ROW,
	V4L2_CID_ROI1_HSUB_SAMPLING,
	V4L2_CID_ROI1_VSUB_SAMPLING,
	V4L2_CID_ROI2_ANA_GAIN,
	V4L2_CID_ROI2_DIG_GAIN,
	V4L2_CID_ROI2_EXPOSURE,
	V4L2_CID_ROI2_HEIGHT,
	V4L2_CID_ROI2_HFLIP,
	V4L2_CID_ROI2_VFLIP,
	V4L2_CID_ROI2_OFFSET_ROW,
	V4L2_CID_ROI2_HSUB_SAMPLING,
	V4L2_CID_ROI2_VSUB_SAMPLING,
	V4L2_CID_TOPAZ_TEST_PATTERN,
	V4L2_CID_IDLE_TOGGLE,
};

struct buffer {
	void   *start;
	size_t  length;
};

static char            *dev_name;
static int              fd = -1;
struct buffer          *buffers;
static unsigned int     n_buffers = 0;
static int              out_buf = 0;
static int              frame_count = DEFAULT_FRAME_COUNT;
int                     discard_queue_buffers = 0;
int                     switched_res = 0;

static void errno_exit(const char *s)
{
	fprintf(stderr, "%s error %d, %s\n", s, errno, strerror(errno));
	exit(EXIT_FAILURE);
}

static int xioctl(int fh, unsigned long int request, void *arg)
{
	int ret;

	do {
		ret = ioctl(fh, request, arg);
	} while (-1 == ret && EINTR == errno);

	return ret;
}

static void set_fmt(int width, int height, int pix_fmt)
{
	struct v4l2_format fmt;

	CLEAR(fmt);

	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width       = width;
	fmt.fmt.pix.height      = height;
	fmt.fmt.pix.pixelformat = pix_fmt;
	fmt.fmt.pix.field       = V4L2_FIELD_NONE;

	if (-1 == xioctl(fd, VIDIOC_S_FMT, &fmt))
		errno_exit("VIDIOC_S_FMT");
}

static int get_ctrl(int control_id)
{
	struct v4l2_control control;

	control.id = control_id;

	if (-1 == xioctl(fd, VIDIOC_G_CTRL, &control))
		errno_exit("VIDIOC_G_CTRL");

	return control.value;
}

static void set_ctrl(int control_id, int value)
{
	struct v4l2_control control;

	control.id = control_id;
	control.value = value;

	if (-1 == xioctl(fd, VIDIOC_S_CTRL, &control))
		errno_exit("VIDIOC_S_CTRL");
}

static void initial_resolution_set_up(void)
{
	set_ctrl(V4L2_CID_ROI_WIDTH, ROI_WIDTH);

	set_ctrl(V4L2_CID_ROI1_HEIGHT, ROI1_HEIGHT);

	set_ctrl(V4L2_CID_ROI2_HEIGHT, ROI2_HEIGHT);

	set_ctrl(V4L2_CID_ROI_SELECT, ROI1_SELECT);

	set_fmt(ROI_WIDTH, ROI1_HEIGHT, DEFAULT_PIX_FMT);
}

static void resolution_switch(void)
{
	set_ctrl(V4L2_CID_IDLE_TOGGLE, 1);

	set_ctrl(V4L2_CID_ROI_SELECT, ROI2_SELECT);

	set_ctrl(V4L2_CID_IDLE_TOGGLE, 0);

	switched_res = 1;
}

static void process_image(const void *p, int size, FILE *fp)
{

	if (out_buf && fp) {
		fwrite(p, size, 1, fp);
	}

	fflush(stderr);
	fprintf(stderr, ".");
	fflush(stdout);
}

static int read_frame(FILE *fp)
{
	struct v4l2_buffer buf;
	unsigned int i;

	CLEAR(buf);

	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;

	if (-1 == xioctl(fd, VIDIOC_DQBUF, &buf)) {
		switch (errno) {
			case EAGAIN:
				return 0;
			default:
				errno_exit("VIDIOC_DQBUF");
		}
	}

	assert(buf.index < n_buffers);

	if (!discard_queue_buffers)
		process_image(buffers[buf.index].start, buf.bytesused, fp);

	if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
		errno_exit("VIDIOC_QBUF");

	return 1;
}

static void mainloop(void)
{
	unsigned int count = 0;
	unsigned int half_count = frame_count / 2;
	FILE *out_file_1 = NULL;
	FILE *out_file_2 = NULL;

	if (out_buf) {
		out_file_1 = fopen(DEFAULT_PATH_1, "wb");
		out_file_2 = fopen(DEFAULT_PATH_2, "wb");
	}

	while (count++ < frame_count) {

		if (!switched_res && (count > half_count)) {
			resolution_switch();
			/* Add V4L2_QUEUE_SIZE so that the total frame count remains the same
			* after discarding V4L2_QUEUE_SIZE buffers */
			frame_count = frame_count + V4L2_QUEUE_SIZE;
		}

		/* When the resolution changes, the queued buffers are trash buffers with
		 * mixed frames of the previous and the current resolution, so they have
		 * to be discarded */
		if ((count <= (half_count + V4L2_QUEUE_SIZE)) && switched_res) {
			discard_queue_buffers = 1;
		} else {
			discard_queue_buffers = 0;
		}

		for (;;) {
			fd_set fds;
			struct timeval tv;
			int ret;
			FILE *out_file = NULL;

			FD_ZERO(&fds);
			FD_SET(fd, &fds);

			/* Timeout. */
			tv.tv_sec = 2;
			tv.tv_usec = 0;

			ret = select(fd + 1, &fds, NULL, NULL, &tv);

			if (-1 == ret) {
				if (EINTR == errno)
					continue;
				errno_exit("select");
			}

			if (0 == ret) {
				fprintf(stderr, "select timeout\n");
				exit(EXIT_FAILURE);
			}

			/* Write first resolution to a file and the second resolution to another */
			if (!switched_res) {
				out_file = out_file_1;
			}
			else {
				out_file = out_file_2;
			}

			if (read_frame(out_file)) {
				break;
			}
		}
	}

	if (out_file_1) {
		fclose(out_file_1);
	}

	if (out_file_2) {
		fclose(out_file_2);
	}
}

static void stop_capturing(void)
{
	enum v4l2_buf_type type;

	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (-1 == xioctl(fd, VIDIOC_STREAMOFF, &type))
		errno_exit("VIDIOC_STREAMOFF");

}

static void start_capturing(void)
{
	unsigned int i;
	enum v4l2_buf_type type;

	for (i = 0; i < n_buffers; ++i) {
		struct v4l2_buffer buf;

		CLEAR(buf);
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = i;

		if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
			errno_exit("VIDIOC_QBUF");
	}
	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (-1 == xioctl(fd, VIDIOC_STREAMON, &type))
		errno_exit("VIDIOC_STREAMON");
}

static void uninit_device(void)
{
	unsigned int i;

	for (i = 0; i < n_buffers; ++i)
		if (-1 == munmap(buffers[i].start, buffers[i].length))
			errno_exit("munmap");

	free(buffers);
}

static void init_mmap(void)
{
	struct v4l2_requestbuffers req;

	CLEAR(req);

	req.count = V4L2_QUEUE_SIZE;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_MMAP;

	if (-1 == xioctl(fd, VIDIOC_REQBUFS, &req)) {
		if (EINVAL == errno) {
			fprintf(stderr, "%s does not support "
				 "memory mapping\n", dev_name);
			exit(EXIT_FAILURE);
		} else {
			errno_exit("VIDIOC_REQBUFS");
		}
	}

	if (req.count < 2) {
		fprintf(stderr, "Insufficient buffer memory on %s\n",
			 dev_name);
		exit(EXIT_FAILURE);
	}

	buffers = calloc(req.count, sizeof(*buffers));

	if (!buffers) {
		fprintf(stderr, "Out of memory\n");
		exit(EXIT_FAILURE);
	}

	for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {
		struct v4l2_buffer buf;

		CLEAR(buf);

		buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory      = V4L2_MEMORY_MMAP;
		buf.index       = n_buffers;

		if (-1 == xioctl(fd, VIDIOC_QUERYBUF, &buf))
			errno_exit("VIDIOC_QUERYBUF");

		buffers[n_buffers].length = buf.length;
		buffers[n_buffers].start =
			mmap(NULL /* start anywhere */,
			      buf.length,
			      PROT_READ | PROT_WRITE /* required */,
			      MAP_SHARED /* recommended */,
			      fd, buf.m.offset);

		if (MAP_FAILED == buffers[n_buffers].start)
			errno_exit("mmap");
	}
}

static void init_device(void)
{
	struct v4l2_capability cap;

	if (-1 == xioctl(fd, VIDIOC_QUERYCAP, &cap)) {
		if (EINVAL == errno) {
			fprintf(stderr, "%s is no V4L2 device\n",
				 dev_name);
			exit(EXIT_FAILURE);
		} else {
			errno_exit("VIDIOC_QUERYCAP");
		}
	}

	if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
		fprintf(stderr, "%s is no video capture device\n",
			 dev_name);
		exit(EXIT_FAILURE);
	}

	if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
		fprintf(stderr, "%s does not support streaming i/o\n",
				dev_name);
		exit(EXIT_FAILURE);
	}

	initial_resolution_set_up();

	init_mmap();
}

static void close_device(void)
{
	if (-1 == close(fd))
		errno_exit("close");

	fd = -1;
}

static void open_device(void)
{
	struct stat st;

	if (-1 == stat(dev_name, &st)) {
		fprintf(stderr, "Cannot identify '%s': %d, %s\n",
			 dev_name, errno, strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (!S_ISCHR(st.st_mode)) {
		fprintf(stderr, "%s is no device\n", dev_name);
		exit(EXIT_FAILURE);
	}

	fd = open(dev_name, O_RDWR /* required */ | O_NONBLOCK, 0);

	if (-1 == fd) {
		fprintf(stderr, "Cannot open '%s': %d, %s\n",
			 dev_name, errno, strerror(errno));
		exit(EXIT_FAILURE);
	}
}

static void usage(FILE *fp, int argc, char **argv)
{
	fprintf(fp,
		 "Usage: %s [options]\n\n"
		 "Version 0.1 (RidgeRun version)\n"
		 "Options:\n"
		 "-d | --device name   Video device name [%s]\n"
		 "-h | --help          Print this message\n"
		 "-o | --output        Save stream to raw files\n"
		 "-c | --count         Number of frames to grab (even number) [%i]\n"
		 "",
		 argv[0], dev_name, frame_count);
}

static const char short_options[] = "d:hofc:";

static const struct option
long_options[] = {
	{ "device", required_argument, NULL, 'd' },
	{ "help",   no_argument,       NULL, 'h' },
	{ "output", no_argument,       NULL, 'o' },
	{ "count",  required_argument, NULL, 'c' },
	{ 0, 0, 0, 0 }
};

int main(int argc, char **argv)
{
	dev_name = "/dev/video0";

	for (;;) {
		int idx;
		int c;

		c = getopt_long(argc, argv,
				short_options, long_options, &idx);

		if (-1 == c)
			break;

		switch (c) {
			case 0: /* getopt_long() flag */
				break;

			case 'd':
				dev_name = optarg;
				break;

			case 'h':
				usage(stdout, argc, argv);
				exit(EXIT_SUCCESS);

			case 'o':
				out_buf++;
				break;

			case 'c':
				errno = 0;
				frame_count = strtol(optarg, NULL, 0);
				if (errno)
					errno_exit(optarg);
				break;

			default:
				usage(stderr, argc, argv);
				exit(EXIT_FAILURE);
		}
	}

	open_device();
	init_device();
	start_capturing();
	mainloop();
	stop_capturing();
	uninit_device();
	close_device();
	fprintf(stderr, "\n");
	return 0;
}
