#include "Protocol5.h"
void Physical_Layer::add_device(Protocol5 *device)
{
	this->devices.push_back(device);
}
void Physical_Layer::send(frame f, int ID)
{
	int num = (rand() % 100);
	if (num > 20)
		this->devices[(ID + 1) % 2]->Physical_layer_queue.push(f);
}