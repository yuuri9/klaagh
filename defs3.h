typedef struct coord coord;
typedef struct point point;

struct coord {
	int x;
	int y;
};
struct point {
	coord;
	uint snext;
	point* next;
};
