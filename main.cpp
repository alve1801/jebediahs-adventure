#define OLC_PGE_APPLICATION
#include <stdio.h>
#include "olc.h"
#include "objs.h"
#define height 100 // 300
#define width 160 // 500

#define stepcount 50
#define stepdelay 20
#define debug true

short newmap[height*width],oldmap[height*width];
d steps[stepcount]; int sp,sc;
d tmp,tmp2;
objs::World rin;
double scale=5;
int speed=1;
double increment=0;
bool pause;

class View:public olc::PixelGameEngine{
	public:View(){sAppName="Jebedian's Adventure";}
	olc::Pixel sga(short color){
		int r = (color&0x0f00)<< 4;
		int g = (color&0x00f0)<< 8;
		int b = (color&0x000f)<<12;
		r=r|r>>4|r>>8|r>>12;
		g=g|g>>4|g>>8|g>>12;
		b=b|b>>4|b>>8|b>>12;
		return olc::Pixel(r,g,b);
	}
  void p(int x,int y,int c){Draw(x,y,sga(c));}
	bool OnUserCreate()override{return true;} // maybe a splashscreen?

	void put(int x,int y,int c){
		tmp.x = x - rin.p.loc.x;
		tmp.y = y - rin.p.loc.y;
		tmp.x/=scale;
		tmp.y/=scale;
		tmp2.x = tmp.x*sin(rin.p.rot*2*M_PI) + tmp.y*cos(rin.p.rot*2*M_PI);
		tmp2.y = tmp.x*cos(rin.p.rot*2*M_PI) - tmp.y*sin(rin.p.rot*2*M_PI);

		if(
			(tmp2.x >-height/2) &&
			(tmp2.x < height/2) &&
			(tmp2.y >-width/2) &&
			(tmp2.y < width/2)
			)
			newmap[((int)tmp2.x+height/2)*width+((int)tmp2.y+width/2)] = c;
	}

	void put(int x,int y,int m,int c){ // for objects w/ mass, it also calculates radius
		// XXX well, it doesnt calculate it properly
		tmp.x = x - rin.p.loc.x;
		tmp.y = y - rin.p.loc.y;
		tmp.x/=scale;
		tmp.y/=scale;
		tmp2.x = tmp.x*sin(rin.p.rot*2*M_PI) + tmp.y*cos(rin.p.rot*2*M_PI);
		tmp2.y = tmp.x*cos(rin.p.rot*2*M_PI) - tmp.y*sin(rin.p.rot*2*M_PI);
		double s = pow(m,.66)/scale*10;

		for(x=-s;x<s;x++)
		for(y=-s;y<s;y++)
		if(
			(tmp2.x+x >-height/2) &&
			(tmp2.x+x < height/2) &&
			(tmp2.y+y >-width/2) &&
			(tmp2.y+y < width/2) &&
			x*x+y*y < s*s/4
			)
			newmap[((int)tmp2.x+x+height/2)*width+((int)tmp2.y+y+width/2)] = c;
	}

	void step(){
		// step world
		// for i in world
		//  for cp in world
		//   if cp!=i
		//    i.dir+=f(bla)
		//  i.loc+=i.dir
		//  p.dir+=f(bla)
		for(int i=0;i<rin.size;i++){
			for(int cp=0;cp<rin.size;cp++)if(cp!=i){
				objs::f(rin.objs[cp].m, rin.objs[i].loc.x, rin.objs[i].loc.y, rin.objs[cp].loc.x, rin.objs[cp].loc.y, &tmp);
				rin.objs[i].dir.x += tmp.x;
				rin.objs[i].dir.y += tmp.y;
			}
			rin.objs[i].loc.x += rin.objs[i].dir.x;
			rin.objs[i].loc.y += rin.objs[i].dir.y;
			// XXX maybe do the thing where the player consists of multiple points and we stimulate for that?
			objs::f(rin.objs[i].m, rin.p.loc.x, rin.p.loc.y, rin.objs[i].loc.x, rin.objs[i].loc.y, &tmp);
			rin.p.dir.x += tmp.x;
			rin.p.dir.y += tmp.y;
		}

		rin.p.loc.x += rin.p.dir.x;
		rin.p.loc.y += rin.p.dir.y;

		// update steps
		sc++;
		if(sc>stepdelay){
			sc=0;
			steps[sp++]=rin.p.loc;
			sp%=stepcount;
		}
	}

	void render(){
		for(int i=0;i<height*width;i++)newmap[i]=0x0000; // clear map
		for(int i=0;i<rin.size;i++)put((int)(rin.objs[i].loc.x),(int)(rin.objs[i].loc.y),rin.objs[i].m,rin.objs[i].c); // draw planets
		for(int i=0;i<stepcount;i++)put(steps[i].x,steps[i].y,0x0ff0); // draw steps
		put(rin.p.loc.x,rin.p.loc.y,0x0fff); // player

		// draw trottle bar
		int x=0;
		for(;x<(height*increment);x++)newmap[(height-x)*width]=0x000f;
		for(;x<height;x++)newmap[(height-x)*width]=0x0000;

		// draw speed bar
		// XXX values largely dependent on original screen ratio, fix that
		double sp = (rin.p.dir.x*rin.p.dir.x+rin.p.dir.y*rin.p.dir.y)*1000;
		double shsp = (log(sp+3)-1)*25;
		if(debug)printf("ship speed:%lf (log:%lf)\n",sp,shsp);
		x=0;
		if(shsp<height){
		for(;x<shsp;x++)
		newmap[(height-x)*width+1]=0x0f00;
		}
		for(;x<height;x++)
		newmap[(height-x)*width+1]=0x0000;

		// draw direction preview
		//put(rin.p.loc.x+10*rin.p.dir.x,rin.p.loc.y+10*rin.p.dir.y,0x00ff);

		/*
		// flight path preview
		objs::Pc a = rin.p;
		for(int i=0;i<stepcount/4;i++){
			for(int j=0;j<stepdelay;j++){
				for(int o=0;o<rin.size;o++){
					objs::f(rin.objs[o].m, a.loc.x, a.loc.y, rin.objs[o].loc.x, rin.objs[o].loc.y, &tmp);
					a.dir.x += tmp.x;
					a.dir.y += tmp.y;
				}
				a.loc.x += a.dir.x;
				a.loc.y += a.dir.y;
			}
			put(a.loc.x,a.loc.y,0xf00);
		}
		*

		for(int i=0;i<rin.size;i++){
			int x = rin.objs[i].loc.x-rin.p.loc.x;
			int y = rin.objs[i].loc.y-rin.p.loc.y;
			double r = (double)(x*x+y*y);
			printf("%g\n",r);
			if(r<1000 || r>1.0e11)break;
			r=sqrt(r)/10;
			x/=r;y/=r;
			printf("r:%g x:%i y:%i\n",r,x,y);
			newmap[(y+height/2)*width+x+width/2]=rin.objs[i].c;
			// XXX we currently ignore size . . .
			// XXX and scale . . .

			newmap[(x+height/2-1)*width+y+width/2-1]=0x0f00;
			newmap[(x+height/2-1)*width+y+width/2+1]=0x0f00;
			newmap[(x+height/2+1)*width+y+width/2-1]=0x0f00;
			newmap[(x+height/2+1)*width+y+width/2+1]=0x0f00;
		}
		*/

		// do that thing where we swap buffers n all
		for(int x=0;x<height;x++)
		for(int y=0;y<width;y++)
		if(oldmap[x*width+y]!=newmap[x*width+y]){
		oldmap[x*width+y]=newmap[x*width+y];
		p(y,x,newmap[x*width+y]);
		}

	}

	bool OnUserUpdate(float fElapsedTime)override{
		// XXX do user io
		if(GetKey(olc::ESCAPE).bPressed)exit(0);
		if(GetKey(olc::F).bPressed)scale*=2;
		if(GetKey(olc::R).bPressed)scale/=2;
		if(scale<.25)scale=.25;
		if(scale>9000)scale=9000;

		if(GetKey(olc::NP_ADD).bPressed)speed*=2;
		if(GetKey(olc::NP_SUB).bPressed && speed>1)speed/=2;
		if(speed>100000)speed=100000;

		if(GetKey(olc::Q).bHeld)rin.p.rot+=.005;
		if(GetKey(olc::E).bHeld)rin.p.rot-=.005;
		if(rin.p.rot<0)rin.p.rot+=1;
		if(rin.p.rot>1)rin.p.rot-=1;

		if(GetKey(olc::SHIFT).bHeld)increment+=.01;
		if(GetKey(olc::CTRL).bHeld)increment-=.01;
		if(GetKey(olc::X).bPressed)increment=0;
		if(GetKey(olc::Z).bPressed)increment=1;
		if(increment<0)increment=0;
		if(increment>1)increment=1;

		if(GetKey(olc::P).bPressed)pause=not pause;

		if(!pause){
			rin.p.dir.x -= sin(rin.p.rot*2*M_PI)*increment/1000;
			rin.p.dir.y -= cos(rin.p.rot*2*M_PI)*increment/1000;
			for(int i=0;i<speed;i++)step();
		}

		if(debug){
			printf("\n~~~\np %lf %lf %f\n",rin.p.loc.x,rin.p.loc.y,rin.p.rot);
			for(int i=0;i<rin.size;i++)
			printf("o %lf %lf\n",rin.objs[i].loc.x,rin.objs[i].loc.y);
			printf("speed:%i zoom:%lf\n",speed,scale);
		}

		render();

		return true;
	}
};

int main(){
	// load world
	rin = objs::testworld();
	View v;if(v.Construct(width,height,5,5))v.Start();
	return(0);
}

/*
basically, a much simplified version of spacecore
objects have loc dir mass size color
ship is a special object which additionally has orientation
	also wireframe?
also plz do that thing where we can save a game n shit

XXX scale up universe
make player consist of multiple vertices
	do we want to bother stimulating that tho
planets have size
XXX each object has its own traces

XXX if we make step take in and return an arg, we could greatly improve the predict function!

wasd controls might be easier . . .

fix the peripheral

*/
