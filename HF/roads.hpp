#pragma once

#include "SF2.hpp"
#include "mat.h"

#include <limits>
#include <vector>
#include <utility>
#include <cmath>
#include <set>
#include <list>

//This graph is made for static terrains, 
//need a rework (Each node knows its own vec2 + search for that point skimming through the vector instead of i * nx + j)
//to be used on dynamic terrains.
struct Node {
	std::vector<std::pair<float,vec2>> Neighbors;

	Node(const std::vector<std::pair<float,vec2>>& tmp): Neighbors(tmp) {}
};
class Graph {
private:

	std::vector<Node> nodes;
	SF2 hei;
	int nx, ny;
	float divx, divy;

	//Only used for terrain init
	void AddNode(const vec2& p,const float& coeff,const float& ms) {
		std::vector<std::pair<float,vec2>> tmp;
		bool b0J = (int)p.y <= 0;
		bool bnJ = (int)p.y >= ny - 1;
		if (! ((int)p.x <= 0)) {
			if (!b0J) {
				tmp.emplace_back(Cost(vec2(-1,-1),p,coeff,ms), vec2(p.x - 1, p.y - 1));
			}
			tmp.emplace_back(Cost(vec2(-1,0),p,coeff,ms), vec2(p.x - 1, p.y));
			if (!bnJ) {
				tmp.emplace_back(Cost(vec2(-1,1),p,coeff,ms), vec2(p.x - 1, p.y + 1));
			}
		}
		if (!b0J) {
			tmp.emplace_back(Cost(vec2(0,-1),p,coeff,ms), vec2(p.x, p.y - 1));
		}
		if (!bnJ) {
			tmp.emplace_back(Cost(vec2(0,1),p,coeff,ms), vec2(p.x, p.y + 1));
		}
		if (! ((int)p.x >= nx - 1)) {
			if (!b0J) {
				tmp.emplace_back(Cost(vec2(1,-1),p,coeff,ms), vec2(p.x + 1, p.y - 1));
			}
			tmp.emplace_back(Cost(vec2(1,0),p,coeff,ms), vec2(p.x + 1, p.y));
			if (!bnJ) {
				tmp.emplace_back(Cost(vec2(1,1),p,coeff,ms), vec2(p.x + 1, p.y + 1));
			}
		}
		nodes.emplace_back(tmp);
	}

	float Cost(const vec2& offset, const vec2& p, const float& coeff,const float& ms) {
		return (1 - coeff) * dist(offset) + coeff * g(offset,slope(offset, p, ms),ms);
	}
	float dist(const vec2& offset) const {
		if ((offset.x == -1 || offset.x == 1) &&  (offset.y == -1 || offset.y == 1)) {
			return std::sqrt(divx * divx + divy * divy);
		}
		else {
			return std::abs(divx * offset.x + divy * offset.y);
		}
	}
	float slope(const vec2& offset, const vec2& p,const float& ms) {
		int ind = p.x * nx + p.y;
		int indneigh = ind + (offset.x * nx + offset.y);
		
		return (hei.at(p.x + offset.x, p.y + offset.y) - hei.at(p)) / dist(offset);
	}
	float g(const vec2& offset, const float& slope,const float& ms) const{
		if (slope > ms) {
			return std::numeric_limits<float>::infinity();
		}
		float res = std::max(slope, 0.f);
		return res;
	}
	bool equal(const vec2& a, const vec2& b) const {
		return a.x == b.x && a.y == b.y;
	}
public:

	Graph() = delete;
	Graph(const SF2& h,const float& coeff,const float& ms) : nx(h.GetNx()), ny(h.GetNy()), divx(h.GetDivx()), divy(h.GetDivy()), hei(h){
		for (int i = 0; i < nx; ++i)
			for (int j = 0; j < ny; ++j)
				AddNode(vec2(i,j),coeff,ms);
	}
	const Node& at(const int& i, const int& j) const {
		return nodes[i * nx + j];
	}
	Node& at(const int& i, const int& j){
		return nodes[i * nx + j];
	}
	const Node& at(const vec2& v) const {
		return nodes[v.x * nx + v.y];
	}
	Node& at(const vec2& v){
		return nodes[v.x * nx + v.y];
	}

	std::list<vec2> Dijkstra(const vec2& start, const  vec2& end) const{
		std::vector<vec2> previous(nx * ny);
		std::vector<float> min_dist(nx * ny, FLT_MAX);
		min_dist[(int)start.x * nx + (int)start.y] = 0;
		auto cmp = [](const std::pair<float,vec2>& a, const std::pair<float,vec2>& b) {
			if (a.first != b.first)
				return a.first < b.first;
			else if (a.second.x != b.second.x)
				return a.second.x < b.second.x;
			else if (a.second.y != b.second.y)
				return a.second.y < b.second.y;
			else
				return false;
		};
		std::set<std::pair<float,vec2>,decltype(cmp)> vecs(cmp);
		vecs.insert(std::make_pair(min_dist[(int)start.x * nx + (int)start.y],start));
		int c = 0;
		while (!vecs.empty()) {
			float dist = vecs.begin()->first;
			vec2 curr = vecs.begin()->second;
			vecs.erase(vecs.begin());
			const auto& neighbors = at(curr).Neighbors;
				for (auto& it : neighbors) {
				float ftmp = it.first;
				vec2 vtmp = it.second;
				float cost = dist + ftmp;
				int i = (int)vtmp.x * nx + (int)vtmp.y;
				if (cost < min_dist[i]) {
					vecs.erase(std::make_pair(min_dist[i], vtmp));
					min_dist[i] = cost;
					previous[i] = curr;
					vecs.insert(std::make_pair(min_dist[i], vtmp));
				}
			}
		}
		std::list<vec2> path;
		for (vec2 v = end; !equal(v, start); v = previous[(int)v.x * nx + (int)v.y]) {
			path.push_front(v);
		}
		return path;
	}
};