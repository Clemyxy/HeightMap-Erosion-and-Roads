#ifndef heightfield
#define heightfield
#include <chrono>
#include "SF2.hpp"
#include "VS.hpp"
#include "mesh.h"
struct Cell {
		vec2 pos;
		float height;
		Cell(const vec2& p, const float& h): pos(p), height(h) {}
		static bool SortCell(const Cell& a, const Cell& b) {
			return a.height > b.height;
		}
	};
class HF2 : public SF2 {
public:
	using SF2::SF2;

	Mesh GetMesh() const {
		Mesh tmp(GL_TRIANGLES);

		float divx = (box.b.x - box.a.x) / (float)nx;
		float divy = (box.b.y - box.a.y) / (float)ny;
		for (int i = 0; i < nx; ++i)
			for (int j = 0; j < ny; ++j) {
				float ijc = at(i, j) / 255;
				tmp.color(ijc,ijc,ijc);
				tmp.vertex(Point(i * divx, at(i,j), j * divy));
			}
		for (int i = 0; i < nx - 1; ++i)
			for (int j = 0; j < ny - 1; ++j) {
				tmp.triangle(i * nx + j, i * nx + j + 1, (i + 1) * nx + j);
				tmp.triangle((i + 1) * nx + j, i * nx + j + 1, (i + 1) * nx + j + 1);
			}
		return tmp;
	}
	void ApplySF(Mesh& mesh, const SF2& sf, const Color& baseCol) const {
		float min = sf.at(0,0), max = sf.at(0,0);
		float range = sf.Range(min,max);
		for (int i = 0; i < mesh.vertex_count(); ++i) {
			mesh.color(i, Color(baseCol * 0.05) + (baseCol * ((sf.at(i) - min) / range)) * 0.8);
		}
	}

	vec3 Normal(const int& i, const int& j) const {
		vec2 grad = Grad(i, j);
		return vec3(-grad.x, -grad.y, 1);
	}
	float S(const int& i, const int& j) const {
		vec2 df = Grad(i, j);
		return sqrt(df.x * df.x + df.y * df.y);
	}
	float S(const vec2& vec) const {
		vec2 df = Grad(vec.x, vec.y);
		return sqrt(df.x * df.x + df.y * df.y);
	}

	SF2 S() const {
		SF2 s(box.a, box.b, nx, ny);
		for(int i = 0; i < nx; ++i)
			for (int j = 0; j < ny; ++j)
				s.at(i,j) = S(i, j);
		return s;
	}
	SF2 S(const float& u, const float& v) const {
		SF2 s = S();
		int di = 1 / u;
		int dj = 1 / v;
		float divx = (box.b.x - box.a.x) / di;
		float divy = (box.b.y - box.a.y) / dj;
		SF2 tmp(box.a, box.b, di, dj);
		for (int i = 0; i < di; ++i) {
			for (int j = 0; j < dj; ++j) {
				tmp.at(i,j) = s.at((float)i/di,(float)j/dj);
			}
		}
				
		return tmp;
	}
	SF2 A(const NeighbouringType& nt) const {
		SF2 tmp(std::vector<float>(nx * ny,1),box.a, box.b, nx, ny);

		std::vector<Cell> cells;
		for (int i = 0; i < nx; ++i)
			for (int j = 0; j < ny; ++j) {
				cells.emplace_back(Cell(vec2(i,j),at(i,j)));
			}

		std::sort(cells.begin(), cells.end(), Cell::SortCell);

		VS vs((SF2)*this);
		
		for (Cell c : cells) {
			if (nt == NeighbouringType::Steepest) {
				if (vs.SteepestValid(c.pos)) {
					vec2 pos = vs.PosSteepest(c.pos);
					tmp.at(pos) += tmp.at(c.pos);
				}
			}
			else if(nt == NeighbouringType::Normalized){
				for (int i = 0; i < 8; ++i) {
					if (vs.Valid(c.pos,i)) {
						vec2 pos = vs.Pos(c.pos,i);
						tmp.at(pos) += tmp.at(c.pos) * vs.NormalizedSlope(c.pos,i);
					}
				}
			}
			else {
				std::cout << "Neighbouring type not recognized" << std::endl;
			}
		}
		return tmp;
	}

	void StreamPowerErode(const NeighbouringType& nt, const float& k) {
		
		SF2 s = S();
		SF2 a = A(nt);
		a.Pow(0.5);
		a.Mul(s);
		a.Mul(-k);
		Add(a);
	}
	//Slower than the echo without n and m, but this function give acces to power parameters.
	void StreamPowerErode(const NeighbouringType& nt, const float& k, const float& n, const float& m) {
		
		SF2 s = S();
		SF2 a = A(nt);
		s.Pow(n);
		a.Pow(m);
		a.Mul(-k);
		a.Mul(s);
		Add(a);
	}
	void HillslopeErode(const float& k) {
		SF2 dZ = Laplacian();
		//dZ.Normalize();
		dZ.Mul(k);
		Add(dZ);
	}
	void DebrisErode(const float& k) {
		SF2 s = S();
		s.Mul(-k);
		Add(s);
	}

};
#endif