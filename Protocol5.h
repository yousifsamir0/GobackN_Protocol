#ifndef PROTOCOL5_H
#define PROTOCOL5_H
#include <queue>
#include <vector>
#include "timer.h"

/* data structures for layer*/
#define MAX_PKT 1 /* determines packet size in bytes */
#define MAX_SEQ 7
#define inc(x) x = (x + 1) % (MAX_SEQ + 1);

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
#include "Physical_Layer.h"
class Protocol5
{

private:
	static int ID;
	static Physical_Layer physical_layer;
	bool network_layer_status;
	unsigned long long timers[MAX_SEQ + 1];
	void from_network_layer(packet *p);
	void to_network_layer(packet *p);
	void from_physical_layer(frame *r);
	void to_physical_layer(frame *s, unsigned int id);
	packet buffer[MAX_SEQ + 1];
	bool is_lastF_data;
	void wait_for_event(event_type *event);
	void start_timer(seq_nr k);
	void stop_timer(seq_nr k);
	void enable_network_layer(void);
	void disable_network_layer(void);
	void send_data(seq_nr frame_nr, seq_nr frame_expected);
	void send_ack(seq_nr frame_nr, seq_nr frame_expected);
	bool check_timeout();
	bool between(seq_nr a, seq_nr b, seq_nr c);

public:
	std::queue<packet> Network_layer;
	std::queue<frame> Physical_layer_queue;
	unsigned int id;

	Protocol5();
	void Start(void);
};

#endif