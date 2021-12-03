#include "Protocol5.h"
#include "windows.h"
#include "omp.h"

void Physical_Layer::add_device(Protocol5 *device)
{
	this->devices.push_back(device);
}
void Physical_Layer::send(frame f, int ID)
{
	int num = (rand() % 100);
	if (num > 20)
	{
		Sleep(180);
		this->devices[(ID + 1) % 2]->Physical_layer_queue.push(f);

		if (omp_get_thread_num() == 0)
		{
			//  for debugging .

			/*printf("sent frame with data: %s from dev(%d) to dev(%d) \n", f.info.data, ID, 1 - ID);
			printf("nbufferd = : %d\n", devices[ID]->nbuffered);
			printf("event is  = : %d\n", devices[ID]->event);
			printf("sender NWLayer q size: %d\n", devices[ID]->Network_layer.size());
			printf("sent frame type: %d\n", f.kind);
			printf("seq number: %d\n", f.seq);
			printf("ack: %d\n", f.ack);
			printf("========================================\n");*/
		}
	}
}