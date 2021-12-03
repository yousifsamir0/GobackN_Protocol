#include "Protocol5.h"
#include <omp.h>
#define MAX_SEQ 7
#define MAX_interval 3000

void Protocol5::inc(seq_nr &x)
{
	x = (x + 1) % (MAX_SEQ + 1);
}

int Protocol5::ID = 0;
Protocol5::Protocol5(Physical_Layer *pl)
{
	id = ID++;
	pl->add_device(this);
	physical_layer = pl;
}
void Protocol5::enable_network_layer()
{
	network_layer_status = 1;
}
void Protocol5::disable_network_layer()
{
	network_layer_status = 0;
}
void Protocol5::to_network_layer(packet *p)
{

	if (p->data[0] != 0)
	{
		printf("Packet received: %c\n", p->data[0]);
		// Network_layer.push(*p);
	}
}
void Protocol5::from_network_layer(packet *p)
{
	packet _packet = Network_layer.front();
	Network_layer.pop();
	p->data[0] = _packet.data[0];
}
void Protocol5::from_physical_layer(frame *f)
{
	frame _frame = Physical_layer_queue.front();
	Physical_layer_queue.pop();
	*f = _frame;
}
void Protocol5::to_physical_layer(frame *f)
{
	physical_layer->send(*f, id);
	// printf("sent frame with data: %c \n", f->info.data[0]);
	// printf("sent frame type: %d\n", f->kind);
}
void Protocol5::send_data(seq_nr frame_nr, seq_nr frame_expected)
{
	/* Construct and send a data frame. */
	frame s;											/* scratch variable */
	s.seq = frame_nr;									/* insert sequence number into frame */
	s.ack = (frame_expected + MAX_SEQ) % (MAX_SEQ + 1); /* piggyback ack */
	s.info = buffer[frame_nr];							/* insert packet into frame */
	s.kind = data;
	to_physical_layer(&s); /* transmit the frame */
	start_timer(frame_nr); /* start the timer running */
}
void Protocol5::send_ack(seq_nr frame_nr, seq_nr frame_expected)
{
	/* Construct and send a ack frame. */
	frame s;											/* scratch variable */
	s.seq = frame_nr;									/* insert sequence number into frame */
	s.ack = (frame_expected + MAX_SEQ) % (MAX_SEQ + 1); /* piggyback ack */
	s.info.data[0] = 0;									/* empty packet */
	s.kind = ack;
	to_physical_layer(&s); /* transmit the frame */
}
void Protocol5::start_timer(seq_nr k)
{
	timers[k] = get_time();
}
void Protocol5::stop_timer(seq_nr k)
{
	timers[k] = 0;
}
bool Protocol5::check_timeout()
{
	long long now = get_time();
	long long t;
	for (int i = 0; i < (MAX_SEQ + 1); i++)
	{
		if (timers[i])
		{
			t = now - timers[i];
			if (t > MAX_interval)
				return 0;
		}
	}
	return 1;
}
bool Protocol5::between(seq_nr a, seq_nr b, seq_nr c)
{
	/* Return true if a <=b < c circularly abc bca cab ; false otherwise. */
	if (((a <= b) && (b < c)) || ((c < a) && (a <= b)) || ((b < c) && (c < a)))
		return (true);
	else
		return (false);
}
void Protocol5::send_message(std::string m)
{
	for (int i = 0; i < m.length(); i++)
	{
		packet p;
		p.data[0] = m[i];

		Network_layer.push(p);
	}
}
/////////////////////////////////

void Protocol5::wait_for_event()
{
	while (1)
	{
		if (omp_get_thread_num() == 1)
			;
		// printf("sneder check_timeout is %d", check_timeout());
		if (network_layer_status && !Network_layer.empty())
		{
			event = network_layer_ready;
			break;
		}
		else if (!check_timeout())
		{
			event = timeout;
			break;
		}
		else if (Physical_layer_queue.empty() && is_lastF_data)
		{
			event = answerack;
			break;
		}
		else if (!Physical_layer_queue.empty())
		{
			event = frame_arrival;
			break;
		}
		else
		{
			event = no_event;
		}
	}
}

///////////////////////////////////

void Protocol5::Start()
{

	is_lastF_data = 0;
	enable_network_layer(); /* allow network_layer_ready events */
	ack_expected = 0;		/* next ack expected inbound */
	next_frame_to_send = 0; /* next frame going out */
	frame_expected = 0;		/* number of frame expected inbound */
	nbuffered = 0;			/* initially no packets are buffered */

	while (true)
	{
		wait_for_event(); /* five possibilities*/

		switch (event)
		{
		case answerack:
			// printf("answer ack");
			is_lastF_data = 0;
			send_ack(next_frame_to_send, frame_expected);
			inc(next_frame_to_send); /* advance sender's upper window edge */
			break;
		case network_layer_ready: /* the network layer has a packet to send */
			/* Accept, save, and transmit a new frame. */
			from_network_layer(&buffer[next_frame_to_send]); /* fetch new packet */
			nbuffered = nbuffered + 1;						 /* expand the sender's window */

			send_data(next_frame_to_send, frame_expected); /* transmit the frame */
			is_lastF_data = 0;							   // sent piggybacked
			inc(next_frame_to_send);					   /* advance sender's upper window edge */

			break;

		case frame_arrival:			 /* a data or control frame has arrived */
			from_physical_layer(&r); /* get incoming frame from physical layer */
			// printf("frame_arr");
			if (r.seq == frame_expected)
			{
				/* Frames are accepted only in order. */
				to_network_layer(&r.info); /* pass packet to network layer */
				inc(frame_expected);	   /* advance lower edge of receiver's window */
				if (r.kind == data)
				{
					is_lastF_data = 1;
				}
			}

			/* Ack n implies n - 1, n - 2, etc.  Check for this. */
			while (between(ack_expected, r.ack, next_frame_to_send))
			{
				/* Handle piggybacked ack. */
				nbuffered = nbuffered - 1; /* one frame fewer buffered */
				stop_timer(ack_expected);  /* frame arrived intact; stop timer */
				inc(ack_expected);		   /* contract sender's window */
			}
			break;

		case timeout:						   /* trouble; retransmit all outstanding frames */
			next_frame_to_send = ack_expected; /* start retransmitting here */
			printf("timeout occured retransmitting from frame: %c ....\n", buffer[next_frame_to_send]);
			for (i = 1; i <= nbuffered; i++)
			{
				send_data(next_frame_to_send, frame_expected); /* resend 1 frame */
				inc(next_frame_to_send);					   /* prepare to send the next one */
			}
		}

		if (nbuffered < MAX_SEQ)
			enable_network_layer();
		else
			disable_network_layer();
	}
}
