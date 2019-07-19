//#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"
//#include "opencv2/core/core.hpp"
//#include "opencv2/imgproc/imgproc.hpp"
#include <math.h>
#include <iostream>
#include <queue>


using namespace cv;
using namespace std;

int length=20,breath=10;


struct point{
	int x;
	int y;
	int parent;
	int angle;
	float g;
	float h;
	float f;
	float r;
	float r2;
};

int mod(int x){
	return(pow(x,2));
}
float w2 = 1800.0,w1 = 0.06;
bool isok(int x,int y,int rows,int cols){
	return (x>0 && y>0 && x<rows && y<cols);
}

int bound(int x,int limit){
	if(x<0) return 0;
	else if(x>=limit) return limit-1;
	else return x;
}

bool isValid(int x,int y,int rows,int cols,Mat img,float t){
	t = t*(3.14/180);
	if(!(x<0||y<0||x>rows||y>cols)){
		int i,j;
		
		if(img.at<Vec3b>(bound(x+length*sin(t),img.rows),bound(y+length*cos(t),img.cols))[0]==255 || img.at<Vec3b>(bound(x+breath*cos(t)+length*sin(t),img.rows),bound(y-breath*sin(t)+length*cos(t),img.cols))[0]==255 || img.at<Vec3b>(bound(x+breath*cos(t),img.rows),bound(y-breath*sin(t),img.cols))[0]==255 || img.at<Vec3b>(x,y)[0]==255) 
		{
			return false;
		}
		else return true;
	}
	else {
		return false;
	}
}

bool operator<(point a,point b)
{
	/*if(a.g!=b.g){
		
		return(a.g>b.g);
	}
	else 
		return(a.f>b.f);*/
	
	//if (a.g!=b.g) 
		return (a.g > b.g);
	//else if(a.r!=b.r) return((a.r)>(b.r));
	//else return(a.r2>b.r2);
}

int absolute(int x){
	if(x==180||x==-180) return 180;
	return abs(((x%180)+180)%180);
}

int mo(int x){
	if(x==180||x==-180) return 180;
	if(x>180) return (x-360);
	else if(x<-180) return (x+360);
	else return x;
}


void generatespace(Mat image,Mat config[])
{
	int i,j,k;
	for(k=0;k<24;k++){
		cout << k << endl;
		for(i=0;i<image.rows;i++){
			for(j=0;j<image.cols;j++){
				if(isValid(i,j,image.rows,image.cols,image,k*15)) config[k].at<Vec3b>(i,j) = {255,0,0};
				else config[k].at<Vec3b>(i,j) = {0,0,0};
				//cout << isValid(image.rows,image.cols,i,j,k*10) <<endl;
			}
		}
	}
}

Mat search(point start,point end,Mat image,Mat config[]){  //Image with black pixel representing obstacle 
	priority_queue<point> q;
	start.f = 0;
	start.g = 0;
	start.h=0;
	start.angle = 22;
	start.parent = 0;
	start.r=w1;
	start.r2 = 0;
	q.push(start);
	cout << "Entered" <<endl;int angle = 0;
	//config[0].at<Vec3b>(start.x,start.y)[1] = 255;
	while(!q.empty()){
		
		point current = q.top();
		int x = current.x;int y = current.y;angle = current.angle;
		if(config[angle].at<Vec3b>(x,y)[1]==255||abs(current.r2)>0.05) {
			
			q.pop();
			continue;
		}
		config[angle].at<Vec3b>(x,y)[1] = 255;
		config[angle].at<Vec3b>(x,y)[2] = current.parent;
		//cout << x << " " << y << endl;
		q.pop();
		//cout << angle << endl;
		if(config[(angle+1)%24].at<Vec3b>(x,y)[0] && !config[(angle+1)%24].at<Vec3b>(x,y)[1]){
			point temp;
			temp.angle = (angle+1)%24;
			temp.x = x;
			temp.y = y;
			temp.f = current.f+w1;
			temp.r *= 2;
			temp.h = sqrt(mod(end.x-temp.x) + mod(temp.y-end.y));
			temp.g = temp.f + temp.h ;
			
			temp.r2 = current.r2;
			temp.parent = 242;
			if(temp.h == 0) {
				config[temp.angle].at<Vec3b>(x,y)[2] =242;
				break;
			}
			q.push(temp);
		}
		if(angle>0){
			if(config[angle-1].at<Vec3b>(x,y)[0] && !config[angle-1].at<Vec3b>(x,y)[1]){
				point temp;
				temp.angle = (angle-1);
				temp.x = x;
				temp.y = y;
				temp.f = current.f+w1;
				temp.r *= 2;
				temp.r2 = current.r2;
				temp.h = sqrt(mod(end.x-temp.x) + mod(temp.y-end.y));
				temp.g = temp.f + temp.h ;
				temp.parent = 244;
				if(temp.h == 0) {
					config[temp.angle].at<Vec3b>(x,y)[2] =244;
					break;
				}
				q.push(temp);
			}
		}
		else if(config[23].at<Vec3b>(x,y)[0] && !config[23].at<Vec3b>(x,y)[1]){
			point temp;
			temp.angle = 23;
			temp.x=x;
			temp.y=y;
			temp.f = current.f+w1;
			temp.r *= 2;
			temp.r2 = current.r2;
			temp.h = sqrt(mod(end.x-temp.x) + mod(temp.y-end.y));
			temp.g = temp.f + temp.h ;
			temp.parent = 244;
			if(temp.h == 0) {
				config[23].at<Vec3b>(x,y)[2] =244;
				break;
			}
			q.push(temp);
		}
		if(isok(x+1,y+1,image.rows,image.cols) && config[angle].at<Vec3b>(x+1,y+1)[0] && !config[angle].at<Vec3b>(x+1,y+1)[1]){
			
			//cout<<"a";
			point temp;
			temp.x = x+1;
			temp.y = y+1;
			temp.angle = angle;
			temp.f = current.f+1.414;
			temp.h = sqrt(mod(end.x-temp.x) + mod(temp.y-end.y));
			
			temp.r2 = current.r2+mo(45-angle*15)/w2;
			temp.g = temp.f + temp.h +abs(temp.r2);
			temp.r = w1;
			//cout<<temp.h<<endl;
			temp.parent = 90;
			if(temp.h == 0) {
				config[angle].at<Vec3b>(x+1,y+1)[2] =90;
				break;
			}
			q.push(temp);
		}
		if(isok(x+1,y-1,image.rows,image.cols) && config[angle].at<Vec3b>(x+1,y-1)[0] && !config[angle].at<Vec3b>(x+1,y-1)[1]){
			//cout<<"b";
			
			point temp;
			temp.x = x+1;
			temp.y = y-1;
			temp.angle = angle;
			temp.f = current.f+1.414;
			temp.r2 =  current.r2 + mo(135-angle*15)/w2;
			temp.h = sqrt(mod(end.x-temp.x) + mod(temp.y-end.y));
			temp.g = temp.f + temp.h +abs(temp.r2);
			temp.r = w1;
			
			temp.parent = 150;
			//cout<<temp.r<<endl;
			if(temp.h == 0) {
				config[angle].at<Vec3b>(x+1,y-1)[2] =150;
				break;
			}
			q.push(temp);
		}
		if(isok(x-1,y-1,image.rows,image.cols) && config[angle].at<Vec3b>(x-1,y-1)[0] && !config[angle].at<Vec3b>(x-1,y-1)[1]){
			//cout<<"c";
			
			point temp;
			temp.x = x-1;
			temp.y = y-1;
			temp.angle = angle;
			temp.f = current.f+1.414;
			temp.r2 = current.r2 + mo(225-angle*15)/w2;
			temp.h = sqrt(mod(end.x-temp.x) + mod(temp.y-end.y)) ;
			temp.g = temp.f + temp.h+ abs(temp.r2);
			temp.r = w1;
			
			//cout<<temp.h<<endl;
			temp.parent = 210;
			if(temp.h == 0) {
				config[angle].at<Vec3b>(x-1,y-1)[2] =210;
				break;
			}
			q.push(temp);
		}
		if(isok(x-1,y+1,image.rows,image.cols) && config[angle].at<Vec3b>(x-1,y+1)[0] && !config[angle].at<Vec3b>(x-1,y+1)[1]){
			//cout<<"d";
			
			point temp;
			temp.x = x-1;
			temp.y = y+1;
			temp.angle = angle;
			temp.f = current.f+1.414;
			temp.r2 =  current.r2 + mo(315-angle*15)/w2;
			temp.h = sqrt(mod(end.x-temp.x) + mod(temp.y-end.y)) ;
			temp.g = temp.f + temp.h + abs(temp.r2);
			temp.r = w1;
			
			//cout<<temp.h<<endl;
			temp.parent = 250;
			if(temp.h == 0) {
				config[angle].at<Vec3b>(x-1,y+1)[2] =250;
				break;
			}
			q.push(temp);
		}
		
		if(isok(x,y+1,image.rows,image.cols) && config[angle].at<Vec3b>(x,y+1)[0] && !config[angle].at<Vec3b>(x,y+1)[1]){
			
			//cout<<"a";
			point temp;
			temp.x = x;
			temp.y = y+1;
			temp.angle = angle;
			temp.f = current.f+1;
			temp.parent = 60;
			temp.r2 = current.r2 + mo(-angle*15)/w2;
			temp.h = sqrt(mod(end.x-temp.x) + mod(temp.y-end.y)) ;
			temp.g = temp.f + temp.h + abs(temp.r2);
			temp.r = w1;
			
			//cout<<temp.h<<endl;
			if(temp.h == 0) {
				config[angle].at<Vec3b>(x,y+1)[2] =60;
				break;
			}
			q.push(temp);
		}
		if(isok(x+1,y,image.rows,image.cols) && config[angle].at<Vec3b>(x+1,y)[0] && !config[angle].at<Vec3b>(x+1,y)[1]){
			//cout<<"b";
			
			point temp;
			temp.x = x+1;
			temp.y = y;
			temp.angle = angle;
			temp.f = current.f+1;
			temp.r2 = current.r2 + mo(90-angle*15)/w2;
			temp.h = sqrt(mod(end.x-temp.x) + mod(temp.y-end.y)) ;
			temp.g = temp.f + temp.h+ abs(temp.r2);
			temp.r = w1;
			
			//cout<<temp.h<<endl;
			if(temp.h == 0) {
				config[angle].at<Vec3b>(x+1,y)[2] =120;
				break;
			}
			temp.parent = 120;
			q.push(temp);
		}
		
		if(isok(x,y-1,image.rows,image.cols) && config[angle].at<Vec3b>(x,y-1)[0] && !config[angle].at<Vec3b>(x,y-1)[1]){
			//cout<<"c";
			
			point temp;
			temp.x = x;
			temp.y = y-1;
			temp.f = current.f+1;
			temp.angle = angle;
			temp.r2 =  mo(180-angle*15)/w2;
			temp.h = sqrt(mod(end.x-temp.x) + mod(temp.y-end.y)) ;
			temp.g = temp.f + temp.h+ abs(temp.r2);
			temp.r = w1;
			
			//cout<<temp.h<<endl;
			temp.parent = 180;
			if(temp.h == 0) {
				config[angle].at<Vec3b>(x,y-1)[2] =180;
				cout << (int)config[angle].at<Vec3b>(x,y-1)[2] <<endl;
				cout << x << " " << y <<endl;
				break;
			}
			q.push(temp);
		}
		if(isok(x-1,y,image.rows,image.cols) && config[angle].at<Vec3b>(x-1,y)[0] && !config[angle].at<Vec3b>(x-1,y)[1]){
			//cout<<"d";
			
			point temp;
			temp.x = x-1;
			temp.y = y;
			temp.f = current.f+1;
			temp.angle = angle;
			temp.r2 = current.r2 + mo(270-angle*15)/w2;
			temp.h = sqrt(mod(end.x-temp.x) + mod(temp.y-end.y)) ;
			temp.g = temp.f + temp.h + abs(temp.r2);
			temp.r = w1;
			
			//cout<<temp.h<<endl;
			temp.parent = 240;
			if(temp.h == 0) {
				config[angle].at<Vec3b>(x-1,y)[2] =240;
				break;
			}
			q.push(temp);
		}
		
	}
	

	cout << "yes" << endl;
	int x=end.x;int y=end.y;
	cout << x << " " <<y <<endl;
	cout << (int)config[angle].at<Vec3b>(x,y)[2] <<endl;	
	cout << !config[angle].at<Vec3b>(x,y)[2] << endl;
	Mat imagef = image.clone();float t=0;
	while(!(x==start.x&&y==start.y)){
		Point vert[4];
		t = angle * 3.14/12;
		cout << "Orientation : " << t*15 << endl;
		cout << "Current position is : " << x << "," << y;
		vert[0] = Point(y,x);vert[1] = Point(y+length*cos(t),x+length*sin(t));vert[3] = Point(y-breath*sin(t),x+breath*cos(t));vert[2] = Point(y+length*cos(t)-breath*sin(t),x+breath*cos(t)+length*sin(t));
		fillConvexPoly(image,vert,4,Scalar(255,255,0));
		imshow("IMG",image);
		waitKey(15);
		fillConvexPoly(image,vert,4,Scalar(0,0,0));
		imagef.at<Vec3b>(x,y) = {0,255,255};
		cout << (int)config[angle].at<Vec3b>(x,y)[2] <<endl;
		switch((int)config[angle].at<Vec3b>(x,y)[2]){
			case 60:
				y-=1;
				break;
			case 90:
				y-=1;
				x-=1;
				break;
			case 120:
				x-=1;
				break;
			case 150:
				x-=1;
				y+=1;
				break;
			case 180:
				y+=1;
				cout << "ok" <<endl;
				break;
			case 210:
				y+=1;
				x+=1;
				break;
			
			case 240:
				x+=1;
				break;
			case 250:
				x+=1;
				y-=1;
				break;
			case 242:
				angle-=1;
				if(angle==-1)angle = 23;
				break;
			case 244:
				angle = (angle+1)%24;
				break;
		}
	}
	cout<<"yes"<<endl;

	return imagef;
}


int main(int argv,char** argc){
	//generatespace();
	namedWindow("IMG",CV_WINDOW_AUTOSIZE);
	Mat image = imread(argc[1],1);
	point start,end;start.x=0;start.y=0;end.x=0;end.y=0;
	int i,j,t1=0,t2=0;
	Mat config[24];
	for(i=0;i<24;i++) config[i] = image.clone();
	generatespace(image,config);
	//imshow("IMG",config[0]);
	//waitKey(0);
	//return 0;
	for(i=0;i<image.rows;i++){
		for(j=0;j<image.cols;j++){
			if(image.at<Vec3b>(i,j)[0] == 0 && image.at<Vec3b>(i,j)[1] ==255 && image.at<Vec3b>(i,j)[2] == 0){
				start.x += i;start.y+=j;t1++;
			}
			if(image.at<Vec3b>(i,j)[0] == 0 && image.at<Vec3b>(i,j)[1] ==0 && image.at<Vec3b>(i,j)[2] == 255){
				end.x += i;end.y+=j;t2++;
			}
		}
	}
	start.x/=t1;start.y/=t1;end.x/=t2;end.y/=t2;
	
	cout << end.x << " " << end.y <<endl;
	//start.x = 0;start.y = 0;end.x = image.rows-100;end.y = image.cols-100;
	image = search(start,end,image,config);	
	cout << "no" <<endl;
	imshow("IMG",image);
	waitKey(0);
	return 0;
}