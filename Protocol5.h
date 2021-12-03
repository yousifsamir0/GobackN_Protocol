#ifndef PROTOCOL5_H
#define PROTOCOL5_H
#include <queue>
#include <vector>
#include "timer.h"
#include "string.h"

/* data structures for layer*/
#define MAX_PKT 1 /* determines packet size in bytes */
#define MAX_SEQ 7

typedef unsigned int seq_nr; /* sequence or ack numbers */
typedef struct
{
	unsigned char data[1];
} packet; /* packet definition */
typedef enum
{
	ack,
	data
} frame_kind; /* frame_kind definition */
typedef struct
{					 /* frames are transported in this layer */
	frame_kind kind; /* what kind of a frame is it? */
	seq_nr seq;		 /* sequence number */
	seq_nr ack;		 /* acknowledgement number */
	packet info;	 /* the network layer packet */
} frame;
typedef enum
{
	no_event = -1,
	answerack,
	frame_arrival,
	timeout,
	network_layer_ready,
} event_type;
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/*Protocol5 class  */
class Physical_Layer;

class Protocol5
{

private:
	seq_nr next_frame_to_send;	/* MAX_SEQ > 1; used for outbound stream */
	seq_nr ack_expected;		/* oldest frame as yet unacknowledged */
	seq_nr frame_expected;		/* next frame expected on inbound stream */
	frame r;					/* scratch variable */
	packet buffer[MAX_SEQ + 1]; /* buffers for the outbound stream */
	seq_nr nbuffered;			/* # output buffers currently in use */
	seq_nr i;					/* used to index into the buffer array */
	event_type event = no_event;
	bool network_layer_status;
	bool is_lastF_data;
	unsigned long long timers[MAX_SEQ + 1];

	static int ID;
	Physical_Layer *physical_layer;
	void from_network_layer(packet *p);
	void to_network_layer(packet *p);
	void from_physical_layer(frame *r);
	void to_physical_layer(frame *s);
	void wait_for_event();
	void start_timer(seq_nr k);
	void stop_timer(seq_nr k);
	void enable_network_layer(void);
	void disable_network_layer(void);
	void send_data(seq_nr frame_nr, seq_nr frame_expected);
	void send_ack(seq_nr frame_nr, seq_nr frame_expected);
	bool check_timeout();
	bool between(seq_nr a, seq_nr b, seq_nr c);
	void inc(seq_nr &x);

public:
	std::string name;
	std::queue<packet> Network_layer;
	std::queue<frame> Physical_layer_queue;
	unsigned int id;

	Protocol5(Physical_Layer *pl);
	void Start(void);
	void send_message(std::string m);

	friend class Physical_Layer;
};

class Physical_Layer
{
public:
	Physical_Layer();
	void add_device(Protocol5 *device);
	void send(frame f, int ID);

private:
	std::vector<Protocol5 *> devices; // assumption-> the use of only two devices
};
#endif