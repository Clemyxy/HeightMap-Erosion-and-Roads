#ifndef neighbours
#define neighbours
#include "SF2.hpp"
#include <algorithm>
#include <chrono>
#include <vector>
enum class NeighbouringType {
		Steepest = 0,
		Normalized = 2
	};

class VS {
private:
	struct Neighborhood {
		float dh[8]  = { 0, 0, 0, 0, 0, 0, 0, 0};	//Height difference
		bool valid[8] = { false, false, false, false, false, false, false, false };
		vec2 offset[8];	//offset
		vec2 pos;
		float s[8];		//Slope percent
		float ns[8];	//Normalized slope percent
		int is = 0;			//index steepest
		int is2 = 0;		//Index of the second steepest slope
		float totHDiff = 1;	//Total height difference
	};
	const vec2 next[8] = { vec2(1, 0), vec2(1, 1), vec2(0, 1), vec2(-1, 1),
					vec2(-1, 0), vec2(-1, -1), vec2(0, -1), vec2(1, -1) };
	int n = 8;
	int nx, ny;
	std::vector<Neighborhood> neighb;
public:
	VS(const SF2& sf) {
		nx = sf.Nx();
		ny = sf.Ny();
		neighb.resize(nx * ny);
		for(int i = 0; i < nx; ++i)
			for (int j = 0; j < ny; ++j) {
				int id = i * nx + j;
				Neighborhood tmp;
				for (int k = 0; k < 8; ++k) {
					tmp.pos = vec2(i, j);
					tmp.valid[k] = true;
					tmp.offset[k] = vec2(i + next[k].x, j + next[k].y);
					if (tmp.offset[k].x < 0 || tmp.offset[k].x > nx - 1 ||
						tmp.offset[k].y < 0 || tmp.offset[k].y > ny - 1) {
						tmp.valid[k] = false;
						continue;
					}
					tmp.dh[k] = sf.at(i, j) - sf.at(tmp.offset[k]);
					if (tmp.dh[k] <= 0) {
						tmp.valid[k] = false;
						continue;
					}
					tmp.s[k] = tmp.dh[k] / sf.at(i,j); //TODO: See if calculation is right

					tmp.totHDiff += tmp.dh[k];
					if (tmp.dh[k] > tmp.dh[tmp.is]) {
						tmp.is2 = tmp.is;
						tmp.is = k;
					}
				}
				for (int k = 0; k < 8; ++k) {
				tmp.ns[k] = tmp.dh[k] / tmp.totHDiff;
				}
				neighb[id] = tmp;
			}
	}

	float Steepest(const vec2& pos) const {
		Neighborhood tmp = neighb[pos.x * nx + pos.y];
		if (tmp.valid[tmp.is])
			return 1;
		else
			return 0;
	}

	float NormalizedSlope(const vec2& pos, const int& i) const {
		Neighborhood tmp = neighb[pos.x * nx + pos.y];
		if (tmp.dh[i] > 0)
			return tmp.ns[i];
		else
			return 0;
	}

	vec2 Pos(const vec2& pos, const int& i) const{
		Neighborhood tmp = neighb[pos.x * nx + pos.y];
		return tmp.offset[i];
	}

	vec2 PosSteepest(const vec2& pos) const {
		Neighborhood tmp = neighb[pos.x * nx + pos.y];
		if (tmp.valid[tmp.is])
			return tmp.offset[tmp.is];
		else
			return tmp.pos;
	}

	bool Valid(const vec2& pos, const int& i) const {
		Neighborhood tmp = neighb[pos.x * nx + pos.y];
		return tmp.valid[i];
	}

	bool SteepestValid(const vec2& pos) const {
		Neighborhood tmp = neighb[pos.x * nx + pos.y];
		return tmp.valid[tmp.is];
	}
};


#endif