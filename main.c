#include <u.h>
#include <libc.h>
#include <bio.h>

#include <thread.h>

#include "defs3.h"

point*
addpoint(point* Point, char* cmd){
	int i;
	switch(cmd[0]){
		case 'U':
			for(i=0;i<atoi(&cmd[1]);++i){
				Point->next = (point*)calloc(1, sizeof(point));
				Point->next->x = Point->x;
				Point->next->y = Point->y + 1;
				Point->snext = 1;
				Point = Point->next;
			}
			return Point;
		case 'L':
			for(i=0;i<atoi(&cmd[1]);++i){
				Point->next = (point*)calloc(1, sizeof(point));
				Point->next->x = Point->x - 1;
				Point->next->y = Point->y;
				Point->snext = 1;
				Point = Point->next;
			}
			return Point;
		case 'R':
			for(i=0;i<atoi(&cmd[1]);++i){
				Point->next = (point*)calloc(1, sizeof(point));
				Point->next->x = Point->x + 1;
				Point->next->y = Point->y;
				Point->snext = 1;
				Point = Point->next;
			}
			return Point;
		case 'D':
			for(i=0;i<atoi(&cmd[1]);++i){
				Point->next = (point*)calloc(1, sizeof(point));
				Point->next->x = Point->x;
				Point->next->y = Point->y - 1;
				Point->snext = 1;
				Point = Point->next;
			}
			return Point;
	}

	fprint(2, "INVALID COMMAND %s\n", cmd);
	return Point;
}

void
wirelistener(void* chn){
	Channel* c, *a;
	char* cmd;
	uint Key;
	point* Point, *Tail, *Recv;

	c = chn;
	a = recvp(c);
	Point = recvp(c);
	Tail = Point;

	for(;;){
		Key = recvul(a);
		switch(Key){
			case 0:
				cmd = recvp(c);
				Tail = addpoint(Tail, cmd);
				free(cmd);
				break;
			case 1:
				for(Tail=Point;;Tail=Tail->next){
					fprint(1, "POINT: %d %d\n", Tail->x, Tail->y);
					if(Tail->snext != 1)
						break;
				}
				sendul(a,1);
				break;

		}
	}

}
void
threadmain( int argc, char** argv){
	point wire[2],*Tail,*Tail2, *Point;
	Channel* c[2], *v[2];
	char* cmd;
	uint i,j;
	int p[2];
	Biobuf* bin, *bcut;

	for(i=0;i<2;++i){
		c[i] = chancreate(sizeof(char*), 1);
		v[i] = chancreate(sizeof(ulong), 1);
		wire[i].x = 0;
		wire[i].y = 0;
		wire[i].snext = 0;
		proccreate(wirelistener,c[i], 2048);
		sendp(c[i], v[i]);
		sendp(c[i], &wire[i]);
	
	}

	pipe(p);
	bin = Bfdopen( 0, OREAD);
	bcut = Bfdopen( p[1], OREAD);

	for(i=0;i<2;++i){
		cmd = Brdstr(bin, '\n',1);

		if(Blinelen(bin) == 0)
			break;

		fprint(p[0], "%s,", cmd);
 		free(cmd);
		
		for(j=0;j<Blinelen(bin);){
			cmd = Brdstr(bcut, ',', 1);
			sendul(v[i], 0);
			chanprint(c[i], "%s", cmd);
			j+= (Blinelen(bcut) + 1);
			free(cmd);
		}
	}

	Point=(point*)calloc(1, sizeof(point));
	Point->x = 0;
	Point->y = 0;


	for(Tail=&wire[0];;Tail=Tail->next){
		for(Tail2=&wire[1];;Tail2=Tail2->next){
			if(Tail->x == Tail2->x && Tail->y == Tail2->y){
				if(Tail->x != 0 && Tail->y != 0){				
					if((Point->x == 0 && Point->y ==0 )|| ((abs(Tail->x) + abs(Tail->y)) < (abs(Point->x) + abs(Point->y))))
						Point = Tail;

					break;
				}

			}

			if(Tail2->snext != 1){
				break;
			}
		}
		if(Tail->snext != 1)
			break;
	}
	fprint(1, "MIN POINT: %d %d\n DISTANCE: %d\n", Point->x, Point->y, (abs(Point->x) + abs(Point->y)));

	threadexitsall(nil);
}
