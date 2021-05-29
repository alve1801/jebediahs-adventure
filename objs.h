#include <math.h>
#define delta 1

struct d{double x;double y;};

namespace objs{
	struct Body{
		d loc;
		d dir;
		int m; // in megatons (1e9kg)
		int s; // size
		short c; // color
	};

	struct Pc{
		d loc;
		d dir;
		float rot;
		float tor; // XXX how tf is this part supposed to work?!?
	};

	struct World{
		Pc p;
		int size;
		Body*objs;
	};

	World testworld(){
		World rin;
		Pc a;
		a.loc={10040,0};
		a.dir={0,.8/delta};
		a.rot=0;
		a.tor=0;
		rin.p=a;
		rin.size=4;
		rin.objs=(Body*)malloc(rin.size*sizeof(Body));

		Body b; // sun
		b.loc={0,0};
		b.dir={0,0};
		b.m=1000;
		b.s=20;
		b.c=0x0ff0;
		rin.objs[0]=b;

		Body c; // planetoid
		c.loc={10000,0};
		c.dir={0,.32/delta};
		c.m=10;
		c.s=5;
		c.c=0x0aaf;
		rin.objs[1]=c;

		Body d; // jovian
		d.loc={-30000,0};
		d.dir={0,-.2/delta};
		d.m=100;
		d.s=5;
		d.c=0x04f4;
		rin.objs[2]=d;

		Body e; // dwarf
		e.loc={400000,0};
		e.dir={0,.05/delta};
		e.m=3;
		e.s=5;
		e.c=0x404; // planet not found
		rin.objs[3]=e;

		return rin;
	}

	void f(int m, double c1x, double c1y, double c2x, double c2y, d*res){
		// do not touch this. it is dark magic.
		double xx,xy,y;
		xx=c2x-c1x;xy=c2y-c1y;
		y=sqrt(xx*xx+xy*xy);
		y=y*y*y;
		res->x=xx*m/y/delta/delta;
		res->y=xy*m/y/delta/delta;
	}
}
