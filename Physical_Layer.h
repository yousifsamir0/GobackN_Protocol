#ifndef PHYSICAL_LAYER_H
#define PHYSICAL_LAYER_H

#include<vector>
#include <stdlib.h>
#include"Protocol5.h"
class Protocol5;
class Physical_Layer{
public:
	void add_device(Protocol5* device);
	void send(frame f,int ID);
private:
	std::vector<Protocol5 *> devices; // assumption-> the use of only two devices
};
#endif