#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <linux/input.h>
#include <pthread.h>
#include <imgui.h>
//#include <draw.h>


static int GetEventCount()
{
    DIR *dir = opendir("/dev/input/");
    struct dirent *ptr = NULL;
    int count = 0;
    while ((ptr = readdir(dir)) != NULL)
    {
        if (strstr(ptr->d_name, "event"))
            count++;
    }
    closedir(dir);
    return count;
}

static int GetEventId(float *Width,float *Height)
{
    int EventCount = GetEventCount();
    struct input_absinfo absX;
    struct input_absinfo absY;
    int fd = -1;
    for (int i = 0; i < EventCount; i++)
    {
        char path[128];
        sprintf(path, "/dev/input/event%d", i);
        int fd = open(path,O_RDWR);
        int ret1 = ioctl(fd, EVIOCGABS(ABS_MT_POSITION_X), &absX);
        int ret2 = ioctl(fd, EVIOCGABS(ABS_MT_POSITION_Y), &absY);
        if(!ret1 && !ret2){
            *Width = absX.maximum + 1;
            *Height = absY.maximum + 1;
            return fd;
        }
    }
    return -1;
}


ImGuiCfg::MyVec2 ImGuiCfg::OScreenTouch(MyVec2 slotPos,float x_p,float y_p){
	MyVec2 Pos;
	switch(Orientation){
		case 0:
			Pos.x = slotPos.x/x_p;
			Pos.y = slotPos.y/y_p;
			return Pos;
		break;
		case 1:
			Pos.x = slotPos.y/y_p;
			Pos.y = screen_y - slotPos.x/x_p;
			return Pos;
		break;
		case 2:
            Pos.x = screen_y - slotPos.x/x_p;
            Pos.y = screen_x - slotPos.y/y_p;
			return Pos;
        break;
		case 3:
        	Pos.x = screen_x - slotPos.y/y_p;
			Pos.y = slotPos.x/x_p;
			return Pos;
        break;
	}
}


void ImGuiCfg::MyTouch(){
	float Width,Height;
	int fd = GetEventId(&Width,&Height);
	if(fd == -1)
	    return;
	
	int scr_x = screen_x;
	int scr_y = screen_y;
	
	if(scr_x > scr_y){
	    int t = scr_y;
	    scr_y = scr_x;
	    scr_x = t;
	}
    
    float x_prp = Width / scr_x;
    float y_prp = Height / scr_y;
    
	ImGuiIO* io = &ImGui::GetIO();
	struct input_event ev;
	int slot = 0;
	size_t len = sizeof(struct input_event);
	while(touchThread){
	    if(read(fd,&ev,len) != len)
			continue;

	    if(ev.type == EV_ABS){
			if(ev.code == ABS_MT_SLOT){
				slot = ev.value;
			}else if(ev.code == ABS_MT_POSITION_X  && slot == 0){
				slotUct[slot].x = ev.value;
	        }else if(ev.code == ABS_MT_POSITION_Y  && slot == 0){
				slotUct[slot].y = ev.value;
	        }else if(ev.code == ABS_MT_TRACKING_ID && slot == 0){
				if(ev.value == -1)
					slotUct[slot].isDown = false;
				else
					slotUct[slot].isDown = true;		
			}
	    }
		
		if(ev.code == SYN_REPORT){
			MyVec2 ImPos = OScreenTouch(slotUct[slot],x_prp,y_prp);
			if(slotUct[slot].isDown){
				io->MouseDown[0] = true;
				io->MousePos = ImVec2(ImPos.x,ImPos.y);
				//printf("%d %.1f  %.1f\n",screen_x,io->MousePos.x,io->MousePos.y);
			}else{
				io->MouseDown[0] = false;
			}
		}
	}
	close(fd);
    return;
}

void ImGuiCfg::StartTouch(){
    thread = std::thread(&ImGuiCfg::MyTouch, this);
    thread.detach();
	usleep(5000);
}
