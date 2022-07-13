// [header]
// A very basic raytracer example.
// [/header]
// [compile]
// c++ -o raytracer -O3 -Wall raytracer.cpp
// [/compile]
// [ignore]
// Copyright (C) 2012  www.scratchapixel.com
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// [/ignore]
#include <stdlib.h>
#include <cstdio>
#include <cmath>
#include <fstream>
#include <vector>
#include <iostream>
#include <cassert>

// Windows only
#include <algorithm>
#include <sstream>
#include <string.h>
#include<chrono>
#include<thread>



#include"JSON_Helper.h"
#include"New_Overrides.h"
#include"Memory_Management.h"
#include"MemoryPool.h"
#if defined __linux__ || defined __APPLE__
// "Compiled for Linux
#else
// Windows doesn't define these values by default, Linux does
#define M_PI 3.141592653589793
#define INFINITY 1e8

#endif




template<typename T>
class Vec3
{
public:
	T x, y, z;
	Vec3() : x(T(0)), y(T(0)), z(T(0)) {}
	Vec3(T xx) : x(xx), y(xx), z(xx) {}
	Vec3(T xx, T yy, T zz) : x(xx), y(yy), z(zz) {}
	Vec3& normalize()
	{
		T nor2 = length2();
		if (nor2 > 0) {
			T invNor = 1 / sqrt(nor2);
			x *= invNor, y *= invNor, z *= invNor;
		}
		return *this;
	}
	Vec3<T> operator * (const T &f) const { return Vec3<T>(x * f, y * f, z * f); }
	Vec3<T> operator * (const Vec3<T> &v) const { return Vec3<T>(x * v.x, y * v.y, z * v.z); }
	T dot(const Vec3<T> &v) const { return x * v.x + y * v.y + z * v.z; }
	Vec3<T> operator - (const Vec3<T> &v) const { return Vec3<T>(x - v.x, y - v.y, z - v.z); }
	Vec3<T> operator + (const Vec3<T> &v) const { return Vec3<T>(x + v.x, y + v.y, z + v.z); }
	Vec3<T>& operator += (const Vec3<T> &v) { x += v.x, y += v.y, z += v.z; return *this; }
	Vec3<T>& operator *= (const Vec3<T> &v) { x *= v.x, y *= v.y, z *= v.z; return *this; }
	Vec3<T> operator - () const { return Vec3<T>(-x, -y, -z); }
	T length2() const { return x * x + y * y + z * z; }
	T length() const { return sqrt(length2()); }
	friend std::ostream & operator << (std::ostream &os, const Vec3<T> &v)
	{
		os << "[" << v.x << " " << v.y << " " << v.z << "]";
		return os;
	}
	//class new overloade
	static void* operator new (size_t size, Heap* heap, string type) {
		cout << "creat new \n";
		cout << "type:" << type << "\n";
		return ::operator new(size, heap);
	}
	static void* operator new[] (size_t size, Heap* heap, int sizeofArray, string type) {
		cout << "Creat new array" << "\n";
		cout << "size of Array"<< sizeofArray << "\n";
		cout << "type:" << type << "\n";
		return ::operator new(size, heap);
	}

	
	static void operator delete (void* pMem) {
		::operator delete(pMem);
	}
	static void operator delete[] (void* pMem) {
		::operator delete(pMem);
	}
	
	
};

typedef Vec3<float> Vec3f;

static const int Width = 640, Height = 480;






class Sphere
{
public:
	Vec3f center;                           /// position of the sphere
	float radius, radius2;                  /// sphere radius and radius^2
	Vec3f surfaceColor, emissionColor;      /// surface color and emission (light)
	float transparency, reflection;         /// surface transparency and reflectivity

	float radiusChange,radiusOrignal;
	Vec3f posMove,centerOrignal;

	Sphere(
		const Vec3f &c,
		const float &r,
		const Vec3f &sc,
		const float &refl = 0,
		const float &transp = 0,
		const Vec3f &ec = 0) :
		center(c), radius(r), radius2(r * r), surfaceColor(sc), emissionColor(ec),
		transparency(transp), reflection(refl),radiusOrignal(r), centerOrignal(c)
	{ /* empty */
	}
	//[comment]
	// Compute a ray-sphere intersection using the geometric solution
	//[/comment]
	bool intersect(const Vec3f &rayorig, const Vec3f &raydir, float &t0, float &t1) const
	{
		Vec3f l = center - rayorig;
		float tca = l.dot(raydir);
		if (tca < 0) return false;
		float d2 = l.dot(l) - tca * tca;
		if (d2 > radius2) return false;
		float thc = sqrt(radius2 - d2);
		t0 = tca - thc;
		t1 = tca + thc;

		return true;
	}

	void updateA(const float &frame){
		radius= lerp(radiusOrignal, radiusChange,frame);
		radius2 = radius * radius;

		center = lerp(centerOrignal, posMove, frame);
	}
	
	float lerp(const float &a, const float &b, const float &f) const
	{
		return a + f * (b - a);
	}
	Vec3f lerp(const Vec3f &a, const Vec3f &b,const Vec3f &f)const
	{
		return a + f * (b - a);
	}

	static void* operator new (size_t size, Heap* heap) {
		cout << "creat new sphere \n";
		
		return ::operator new(size, heap);
	}
	static void operator delete (void* pMem) {
		::operator delete(pMem);
	}
	
};

//[comment]
// This variable controls the maximum recursion depth
//[/comment]
#define MAX_RAY_DEPTH 5

float mix(const float &a, const float &b, const float &mix)
{
	return b * mix + a * (1 - mix);
}

//[comment]
// This is the main trace function. It takes a ray as argument (defined by its origin
// and direction). We test if this ray intersects any of the geometry in the scene.
// If the ray intersects an object, we compute the intersection point, the normal
// at the intersection point, and shade this point using this information.
// Shading depends on the surface property (is it transparent, reflective, diffuse).
// The function returns a color for the ray. If the ray intersects an object that
// is the color of the object at the intersection point, otherwise it returns
// the background color.
//[/comment]
Vec3f trace(
	const Vec3f &rayorig,
	const Vec3f &raydir,
	const std::vector<Sphere*> &spheres,
	const int &depth)
{
	//if (raydir.length() != 1) std::cerr << "Error " << raydir << std::endl;
	float tnear = INFINITY;
	const Sphere* sphere = NULL;
	// find intersection of this ray with the sphere in the scene

	for (unsigned i = 0; i < spheres.size(); ++i) {
		float t0 = INFINITY, t1 = INFINITY;
		if (spheres[i]->intersect(rayorig, raydir, t0, t1)) {
			if (t0 < 0) t0 = t1;
			if (t0 < tnear) {
				tnear = t0;
				sphere = spheres[i];
			}
		}
	}

	// if there's no intersection return black or background color
	if (!sphere) return Vec3f(2);
	Vec3f surfaceColor = 0; // color of the ray/surfaceof the object intersected by the ray
	Vec3f phit = rayorig + raydir * tnear; // point of intersection
	Vec3f nhit = phit - sphere->center; // normal at the intersection point
	nhit.normalize(); // normalize normal direction
					  // If the normal and the view direction are not opposite to each other
					  // reverse the normal direction. That also means we are inside the sphere so set
					  // the inside bool to true. Finally reverse the sign of IdotN which we want
					  // positive.
	float bias = 1e-4; // add some bias to the point from which we will be tracing
	bool inside = false;
	if (raydir.dot(nhit) > 0) nhit = -nhit, inside = true;
	if ((sphere->transparency > 0 || sphere->reflection > 0) && depth < MAX_RAY_DEPTH) {
		float facingratio = -raydir.dot(nhit);
		// change the mix value to tweak the effect
		float fresneleffect = mix(pow(1 - facingratio, 3), 1, 0.1);
		// compute reflection direction (not need to normalize because all vectors
		// are already normalized)
		Vec3f refldir = raydir - nhit * 2 * raydir.dot(nhit);
		refldir.normalize();
		Vec3f reflection = trace(phit + nhit * bias, refldir, spheres, depth + 1);
		Vec3f refraction = 0;
		// if the sphere is also transparent compute refraction ray (transmission)
		if (sphere->transparency) {
			float ior = 1.1, eta = (inside) ? ior : 1 / ior; // are we inside or outside the surface?
			float cosi = -nhit.dot(raydir);
			float k = 1 - eta * eta * (1 - cosi * cosi);
			Vec3f refrdir = raydir * eta + nhit * (eta *  cosi - sqrt(k));
			refrdir.normalize();
			refraction = trace(phit - nhit * bias, refrdir, spheres, depth + 1);
		}
		// the result is a mix of reflection and refraction (if the sphere is transparent)
		surfaceColor = (
			reflection * fresneleffect +
			refraction * (1 - fresneleffect) * sphere->transparency) * sphere->surfaceColor;
	}
	else {
		// it's a diffuse object, no need to raytrace any further
		for (unsigned i = 0; i < spheres.size(); ++i) {
			if (spheres[i]->emissionColor.x > 0) {
				// this is a light
				Vec3f transmission = 1;
				Vec3f lightDirection = spheres[i]->center - phit;
				lightDirection.normalize();
				for (unsigned j = 0; j < spheres.size(); ++j) {
					if (i != j) {
						float t0, t1;
						if (spheres[j]->intersect(phit + nhit * bias, lightDirection, t0, t1)) {
							transmission = 0;
							break;
						}
					}
				}
				surfaceColor += sphere->surfaceColor * transmission *
					std::max(float(0), nhit.dot(lightDirection)) * spheres[i]->emissionColor;
			}
		}
	}

	return surfaceColor + sphere->emissionColor;
}

static PoolAllocator<sizeof(Vec3f[Width * Height]), 1>* RenderImmagePool;
mutex Lock;
void Raytrace(Vec3f* pixel,const std::vector<Sphere*>& spheres, int height, int width , float invWidth,float invHeight,float angle, float aspectratio,int yI,int xI) {

	for (unsigned y = yI; y < height; ++y) {
		for (unsigned x = xI; x < width; ++x, ++pixel) {
			float xx = (2 * ((x + 0.5) * invWidth) - 1) * angle * aspectratio;
			float yy = (1 - 2 * ((y + 0.5) * invHeight)) * angle;
			Vec3f raydir(xx, yy, -1);
			raydir.normalize();
			
			*pixel = trace(Vec3f(0), raydir, spheres, 0);
		}
	}
}

//[comment]
// Main rendering function. We compute a camera ray for each pixel of the image
// trace it and return a color. If the ray hits a sphere, we return the color of the
// sphere at the intersection point, else we return the background color.
//[/comment]
void render(const std::vector<Sphere*> &spheres, int iteration)
{
	// quick and dirty
	unsigned width = Width, height = Height;
	// Recommended Testing Resolution
	//unsigned width = 640, height = 480;

	// Recommended Production Resolution
	//unsigned width = 1920, height = 1080;
	//alocate data in memory pool
	Vec3f *image = (Vec3f*)RenderImmagePool->Allocate() , * pixel = image;

	float invWidth = 1 / float(width), invHeight = 1 / float(height);
	float fov = 30, aspectratio = width / float(height);
	float angle = tan(M_PI * 0.5 * fov / 180.);

	int numberOfThreads = 4;

	//create threads
	thread t1 (Raytrace,pixel, spheres, height/ numberOfThreads, width, invWidth, invHeight, angle, aspectratio,0,0);
	thread t2 (Raytrace,pixel+ (height / numberOfThreads) * width, spheres, (height/4)*2, width, invWidth, invHeight, angle, aspectratio, (height / numberOfThreads),0);
	thread t3 (Raytrace,pixel+ ((height / numberOfThreads) * width)*2, spheres, (height / 4) * 3, width, invWidth, invHeight, angle, aspectratio, (height / numberOfThreads)*2, 0);
	thread t4 (Raytrace,pixel+ ((height / numberOfThreads) * width)*3, spheres, (height / 4) * 4, width, invWidth, invHeight, angle, aspectratio,(height / numberOfThreads)*3,0);


	//join threads
	t1.join();
	t2.join();
	t3.join();
	t4.join();


	// Save result to a PPM image (keep these flags if you compile under Windows)
	std::stringstream ss;
	ss << "./spheres" << iteration << ".ppm";
	std::string tempString = ss.str();
	char* filename = (char*)tempString.c_str();

	//C style file load
	FILE* data;
	data = fopen(filename, "wb");

	if (data != NULL)
	{


		string headderString = "P6\n" + to_string(width) + " " + to_string(height) + "\n255\n";
		std::fwrite(headderString.c_str(), sizeof(char), headderString.size(), data);

		unsigned char outImmage[(Width * Height) * 3];
		int dataPosition = 0;
		for (unsigned i = 0; i < width * height; ++i) {
			outImmage[dataPosition] = (unsigned char)(std::min(float(1), image[i].x) * 255);
			++dataPosition;
			outImmage[dataPosition] = (unsigned char)(std::min(float(1), image[i].y) * 255);
			++dataPosition;
			outImmage[dataPosition] = (unsigned char)(std::min(float(1), image[i].z) * 255);
			++dataPosition;
		}

		std::fwrite(outImmage, sizeof(unsigned char), sizeof(outImmage), data);


		std::fclose(data);
	}
	RenderImmagePool->GetAllData();

	//dealocate memory from immage pool
	RenderImmagePool->Deallocate(image);

	RenderImmagePool->GetAllData();
	Memory_Management::GetDefaultHeap()->GetAllData();
}

void SmoothScaling()
{

	//set up object pool
	RenderImmagePool = new PoolAllocator< sizeof(Vec3f[Width * Height]),1>();

	//error with this vector removes itslef???
	//std::vector<Sphere> spheres;
	std::vector<Sphere*> spheres1;
	// Vector structure for Sphere (position, radius, surface color, reflectivity, transparency, emission color)

	//load data from json
	std::vector<SphereData> data= JSON_Helper::LoadJsonFile("Test1.json");

	for (const auto& data1 : data) {
		spheres1.push_back(new(Memory_Management::GetHeap("RenderObjects")) Sphere(Vec3f(data1.Position[0], data1.Position[1], data1.Position[2]), data1.radius, Vec3f(data1.surface_Color[0], data1.surface_Color[1], data1.surface_Color[2]),
			data1.reflectivity, data1.transparency, Vec3f(data1.emission_Color[0], data1.emission_Color[1], data1.emission_Color[2])));

		spheres1.back()->radiusChange = data1.radiusChange;
		spheres1.back()->posMove = Vec3f(data1.PositionMove[0], data1.PositionMove[1], data1.PositionMove[2]);

	}

	int frame = 100;
	for (float r = 0; r <= frame; r++)
	{
	

		for (Sphere* sphere : spheres1) {
			sphere->updateA(r/ frame);
		}
		

		render(spheres1, r);


		std::cout << "Rendered and saved spheres" << r << ".ppm" << std::endl;


	}


	Memory_Management::GetHeap("RenderObjects")->GetAllData();
	Memory_Management::GetDefaultHeap()->GetAllData();

	for (Sphere* sphere : spheres1) {
		delete sphere;
	}



	spheres1.clear();


	delete RenderImmagePool;
}
//[comment]
// In the main function, we will create the scene which is composed of 5 spheres
// and 1 light (which is also a sphere). Then, once the scene description is complete
// we render that scene, by calling the render() function.
//[/comment]
int main(int argc, char **argv)
{

	//creat heaps 
	Memory_Management::AddHeap("RenderObjects");
	//start check
	Memory_Management::GetDefaultHeap()->GetAllData();

	auto start = std::chrono::steady_clock::now();

	SmoothScaling();

	auto finish = std::chrono::steady_clock::now();
	double elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(finish - start).count();
	cout << elapsed_seconds << endl;

	//end check
	Memory_Management::GetHeap("RenderObjects")->GetAllData();

	//clear all heaps in list but not defualt 
	Memory_Management::RemoveAllHeap();

	Memory_Management::GetDefaultHeap()->GetAllData();

	
	return 0;
}


