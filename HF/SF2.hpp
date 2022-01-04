#ifndef scalarfield
#define scalarfield

#include <vector>
#include "vec.h"
#include <algorithm>
struct Box2 {
	vec2 a;
	vec2 b;
};
class SF2 {
private:
	const float& lerp(float s, float e, float t) const {return s+(e-s)*t;}
	const float& blerp(float c00, float c10, float c01, float c11, float tx, float ty) const{
		return lerp(lerp(c00, c10, tx), lerp(c01, c11, tx), ty);
	}
	float blerp(float c00, float c10, float c01, float c11, float tx, float ty){
		return lerp(lerp(c00, c10, tx), lerp(c01, c11, tx), ty);
	}

protected:
	std::vector<float> v;
	Box2 box;
	int nx, ny;
	float dx, dy;
	
public:
	SF2(const int& nbx, const int& nby): nx(nbx), ny(nby) {
		box.a = vec2(0, 0);
		box.b = vec2(nbx,nby);
		v.resize(nbx * nby);
		dx = (box.b.x - box.a.x) / (nx - 1);
		dy = (box.b.y - box.a.y) / (ny - 1);
	}
	SF2(const vec2& min, const vec2& max,const int& nbx, const int& nby): nx(nbx), ny(nby) {
		box.a = min;
		box.b = max;
		v.resize(nbx * nby);
		dx = (box.b.x - box.a.x) / (nx - 1);
		dy = (box.b.y - box.a.y) / (ny - 1);
	}
	SF2(const std::vector<float>& data, const vec2& min, const vec2& max, const int& nbx, const int& nby): v(data), nx(nbx), ny(nby) {
		box.a = min;
		box.b = max;
		dx = (box.b.x - box.a.x) / (nx - 1);
		dy = (box.b.y - box.a.y) / (ny - 1);
	}

	int Nx() const { return nx; }
	int Ny() const { return ny; }

	vec2 Grad(const int& i, const int& j) const {
		vec2 tmp;
		if (i == 0)
			tmp.x = (v[(i + 1) * nx + j] - v[j]) / dx;
		else if (i == nx - 1)
			tmp.x = (v[i * nx + j] - v[(i - 1) * nx + j]) / dx;
		else
			tmp.x = (v[(i + 1) * nx + j] - v[(i - 1) * nx + j]) / (2 * dx);
		if (j == 0)
			tmp.y = (v[i * nx + j + 1] - v[i * nx + j]) / dy;
		else if (j == ny - 1)
			tmp.y = (v[i * nx + j] - v[i * nx + j - 1]) / dy;
		else
			tmp.y = (v[i * nx + j + 1] - v[i * nx + j - 1]) / (2 * dy);
		
		return tmp;
	}

	float Laplacian(const int& i, const int& j) const {
		float vi;
		float vj;
		if (i == 0)
			vi = (v[j] - 2 * v[j] + v[nx + j]) / (dx * dx);
		else if (i == nx - 1)
			vi = (v[(i - 1) * nx + j] - 2 * v[i * nx + j] + v[i * nx + j]) / (dx * dx);
		else
			vi = (v[(i - 1) * nx + j] - 2 * v[i * nx + j] + v[(i + 1) * nx + j]) / (dx * dx);
		if (j == 0)
			vj = (v[i * nx + j] - 2 * v[i * nx + j] + v[i * nx + j + 1]) / (dy * dy);
		else if (j == ny - 1)
			vj = (v[i * nx + j - 1] - 2 * v[i * nx + j] + v[i * nx + j]) / (dy * dy);
		else
			vj = (v[i * nx + j + 1] - 2 * v[i * nx + j] +  v[i * nx + j - 1]) / (dy * dy);
		return vi + vj;
	}

	SF2 Laplacian() const {
		SF2 dZ(box.a, box.b, nx, ny);
		for(int i = 0; i < nx; ++i)
			for (int j = 0; j < ny; ++j) 
				dZ.at(i,j) = Laplacian(i, j);
		return dZ;
	}

	void Clamp(const float& min, const float& max) {
		for(int i = 0; i < nx; ++i)
			for (int j = 0; j < ny; ++j) {
				at(i, j) = std::max(min,std::min(max,at(i, j)));
			}
	}

	float& at(const int& i, const int& j) { return v[i * nx + j]; }
	const float& at(const int& i, const int& j) const{ return v[i * nx + j]; }

	float& at(const int& i) { return v[i]; }
	const float& at(const int& i) const{ return v[i]; }
	float& at(const vec2& vec) { return v[vec.x * nx + vec.y]; }
	const float& at(const vec2& vec) const{ return v[vec.x * nx + vec.y]; }

	void Sqrt() {
		for (float &d : v)
			d = sqrt(d);
	}
	
	void Add(const SF2& sf) {
		for(int i = 0; i < nx; ++i)
			for (int j = 0; j < ny; ++j)
				at(i, j) = std::max(0.f,at(i, j) + sf.at(i, j));	
	}

	void Mul(const float& f) {
		for (float &d : v)
			d = d * f;
	}
	void Mul(const SF2& sf) {
		for(int i = 0; i < nx; ++i)
			for (int j = 0; j < ny; ++j)
				at(i, j) = at(i, j) * sf.at(i, j);	
	}

	void Pow(const float& a) {
		for (float &d : v)
			d = std::pow(d,a);
	}

	float Range(float& min, float& max) const{
		for (float d : v) {
			min = std::min(d, min);
			max = std::max(d, max);
		}
		return max - min;
	}
	float Range() const{
		float min = v[0];
		float max = v[0];
		for (float d : v) {
			min = std::min(d, min);
			max = std::max(d, max);
		}
		return max - min;
	}

	void Normalize() {
		float min, max;
		float range = Range(min, max);
		for (float& d : v)
			d = d / range;
	}
	int GetNx() const{ return nx; }
	int GetNy() const{ return ny; }
	float GetDivx() const{ return (box.b.x - box.a.x) / nx; }
	float GetDivy() const{ return (box.b.y - box.a.y) / ny; }
};
#endif