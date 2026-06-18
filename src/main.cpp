#include <vector>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <random>
#include <chrono>
#include <map>
#include <algorithm>

#include "../Point3D.h"
#include "../Element.h"
#include "../Hexahedron.h"
#include "../Prism.h"
#include "../Pyramid.h"
#include "../Tetrahedron.h"

using namespace std;
using Clobscode::Point3D;

class MeshRandomizer {
public:
	MeshRandomizer() {}
	~MeshRandomizer() {
		for (size_t i = 0; i < elements.size(); ++i) {
			if (elements[i]) delete elements[i];
		}
	}

	// read/write .m3d (same format used elsewhere in the project)
	bool readM3d(const std::string &name) {
		char word[256];
		int pcant = 0, ecant = 0;
		double x = 0, y = 0, z = 0;
		bool mvm = false;

		FILE *file = fopen(name.c_str(), "r");
		if (file == NULL) {
			std::cerr << "File " << name << " doesn't exist\n";
			return false;
		}

		// read number of nodes
		while (true) {
			if (std::fscanf(file, "%s", word) == EOF) {
				fclose(file);
				return false;
			}
			if (!strcmp(word, "ARRAY1<STRING>]\0"))
				break;
			if (!strcmp(word, "MIXED\0")) {
				mvm = true;
				break;
			}
		}

		std::fscanf(file, "%i", &pcant);
		if (pcant <= 0) {
			fclose(file);
			return false;
		}

		if (mvm) {
			std::fscanf(file, "%i", &ecant);
			if (ecant <= 0) {
				fclose(file);
				return false;
			}
		}

		// read each node
		points.clear();
		points.reserve(pcant);
		node2elem_map.clear();
		node2elem_map.resize(pcant);

		for (int i = 0; i < pcant; i++) {
			if (!mvm) {
				std::fscanf(file, "%s", word);
			}
			std::fscanf(file, "%s", word);
			x = atof(word);
			std::fscanf(file, "%s", word);
			y = atof(word);
			std::fscanf(file, "%s", word);
			z = atof(word);
			Point3D p(x, y, z);
			points.push_back(p);
		}

		// read number of elements
		if (!mvm) {
			while (1) {
				if (std::fscanf(file, "%s", word) == EOF) {
					std::cerr << "didn't find elements\n";
					fclose(file);
					return false;
				}

				if (!strcmp(word, "ARRAY1<STRING>]\0")) {
					std::fscanf(file, "%i", &ecant);
					break;
				}
			}

			if (ecant <= 0) {
				fclose(file);
				return false;
			}
		}

		elements.clear();
		elements.reserve(ecant);

		// read each element
		for (int i = 0; i < ecant; i++) {
			std::fscanf(file, "%s", word);

			if (!strcmp(word, "H\0")) {
				std::vector<int> element(8, 0);
				for (unsigned int j = 0; j < 8; j++) {
					fscanf(file, "%i", &element[j]);
					if ((size_t)element[j] >= node2elem_map.size()) node2elem_map.resize(element[j]+1);
					node2elem_map[element[j]].push_back(i);
				}
				elements.push_back(new Hexahedron(element));
			}
			else if (!strcmp(word, "R\0")) {
				std::vector<int> element(6, 0);
				for (unsigned int j = 0; j < 6; j++) {
					fscanf(file, "%i", &element[j]);
					if ((size_t)element[j] >= node2elem_map.size()) node2elem_map.resize(element[j]+1);
					node2elem_map[element[j]].push_back(i);
				}
				elements.push_back(new Prism(element));
			}
			else if (!strcmp(word, "P\0")) {
				std::vector<int> element(5, 0);
				for (unsigned int j = 0; j < 5; j++) {
					fscanf(file, "%i", &element[j]);
					if ((size_t)element[j] >= node2elem_map.size()) node2elem_map.resize(element[j]+1);
					node2elem_map[element[j]].push_back(i);
				}
				elements.push_back(new Pyramid(element));
			}
			else if (!strcmp(word, "T\0")) {
				std::vector<int> element(4, 0);
				for (unsigned int j = 0; j < 4; j++) {
					fscanf(file, "%i", &element[j]);
					if ((size_t)element[j] >= node2elem_map.size()) node2elem_map.resize(element[j]+1);
					node2elem_map[element[j]].push_back(i);
				}
				elements.push_back(new Tetrahedron(element));
			}
			else {
				cerr << "unknown element type at element " << i << "\n";
				fclose(file);
				return false;
			}

			// read some unnecessary integers
			if (!mvm) {
				for (unsigned int j = 0; j < 3; j++)
					std::fscanf(file, "%s", word);
			}
		}
		fclose(file);

		return true;
	}

	bool writeM3d(const std::string &name) {
		if (elements.empty()) {
			std::cerr << "no output elements\n";
			return false;
		}

		string vol_name = name + ".m3d";

		// write the volume mesh
		FILE *f = fopen(vol_name.c_str(), "wt");
		if (!f) {
			std::cerr << "Cannot open file " << vol_name << " for writing\n";
			return false;
		}

		unsigned int n = points.size();

		fprintf(f, "%s\n", "[Nodes, ARRAY1<STRING>]");
		fprintf(f, "%i\n\n", n);

		// write points
		for (unsigned int i = 0; i < n; i++) 
		{
			fprintf(f, "1 %+1.8E", points[i][0]);
			fprintf(f, " %+1.8E", points[i][1]);
			fprintf(f, " %+1.8E\n", points[i][2]);
		}

		n = elements.size();

		fprintf(f, "\n%s\n", "[Elements, ARRAY1<STRING>]");
		fprintf(f, "%u\n\n", n);

		// get all the elements
		for (unsigned int i = 0; i < n; i++) {
			std::vector<unsigned int> epts = elements[i]->getPoints();
			unsigned int np = epts.size();
			if (np == 4) {
				fprintf(f, "T");
			}
			else if (np == 5) {
				fprintf(f, "P");
			}
			else if (np == 6) {
				fprintf(f, "R");
			}
			else if (np == 8) {
				fprintf(f, "H");
			}

			for (unsigned int j = 0; j < np; j++) {
				fprintf(f, " %u", epts.at(j));
			}

			fprintf(f, " 1000.0 0.45 1.0\n");
		}
		fclose(f);
		return true;
	}

	bool writeVtk(const std::string &name) {
		if (elements.empty()) {
			std::cerr << "no output elements\n";
			return false;
		}

		string vol_name = name + ".vtk";

		FILE *f = fopen(vol_name.c_str(), "wt");
		if (!f) {
			std::cerr << "Cannot open file " << vol_name << " for writing\n";
			return false;
		}

		fprintf(f, "# vtk DataFile Version 2.0\nUnstructured Grid %s\nASCII", name.c_str());
		fprintf(f, "\n\nDATASET UNSTRUCTURED_GRID\nPOINTS %u float", (unsigned int)points.size());

		// write points
		for (unsigned int i = 0; i < points.size(); i++) {
			if (i % 2 == 0) {
				fprintf(f, "\n");
			}
			fprintf(f, " %+1.8E", points[i][0]);
			fprintf(f, " %+1.8E", points[i][1]);
			fprintf(f, " %+1.8E", points[i][2]);
		}

		// count connectivity index.
		unsigned int conectivity = 0;
		for (unsigned int i = 0; i < elements.size(); i++) {
			conectivity += elements[i]->getPoints().size() + 1;
		}

		fprintf(f, "\n\nCELLS %u %u\n", (unsigned int)elements.size(), conectivity);

		// get all the elements
		for (unsigned int i = 0; i < elements.size(); i++) {
			std::vector<unsigned int> epts = elements[i]->getPoints();
			unsigned int np = epts.size();
			fprintf(f, "%u", np);

			if (np == 6) {
				unsigned int aux = epts[1];
				epts[1] = epts[2];
				epts[2] = aux;
				aux = epts[4];
				epts[4] = epts[5];
				epts[5] = aux;
			}

			for (unsigned int j = 0; j < np; j++) {
				fprintf(f, " %u", epts.at(j));
			}

			fprintf(f, "\n");
		}

		fprintf(f, "\nCELL_TYPES %u", (unsigned int)elements.size());
		for (unsigned int i = 0; i < elements.size(); i++) {
			if (i % 30 == 0) { fprintf(f, "\n"); }
			unsigned int np = elements[i]->getPoints().size();
			if (np == 4) {
				fprintf(f, "10 ");
			}
			else if (np == 5) {
				fprintf(f, "14 ");
			}
			else if (np == 6) {
				fprintf(f, "13 ");
			}
			else if (np == 8) {
				fprintf(f, "12 ");
			}
		}
		fprintf(f, "\n");

		fclose(f);
		return true;
	}


	std::vector<bool> detectSurfaceNodes() {
		std::vector<bool> is_on_surface(points.size(), false);
		std::map<std::vector<unsigned int>, int> face_counts;

		for (size_t i = 0; i < elements.size(); ++i) {
			Element* elem = elements[i];
			unsigned int num_faces = elem->numberOfFaces();
			for (unsigned int j = 0; j < num_faces; ++j) {
				std::vector<unsigned int> face_nodes = elem->getFacePoints(j);
				std::sort(face_nodes.begin(), face_nodes.end());
				face_counts[face_nodes]++;
			}
		}

		for (auto const& pair : face_counts) {
			if (pair.second == 1) {
				for (unsigned int node_idx : pair.first) {
					if (node_idx < is_on_surface.size()) {
						is_on_surface[node_idx] = true;
					}
				}
			}
		}

		return is_on_surface;
	}

	// randomize nodes: add uniform random displacement inside sphere of radius d * local_average_edge_length
	void randomizeNodes(double d, unsigned int seed = 0, bool keep_surface_fixed = false) {
		if (d <= 0.0) return;
		unsigned int use_seed = seed;
		if (use_seed == 0) {
			std::random_device rd;
			use_seed = rd();
		}
		std::mt19937 rng(use_seed);
		std::uniform_real_distribution<double> uni(-1.0, 1.0);

		std::vector<bool> is_on_surface;
		if (keep_surface_fixed) {
			is_on_surface = detectSurfaceNodes();
		} else {
			is_on_surface.assign(points.size(), false);
		}

		std::vector<std::vector<unsigned int>> neighbors(points.size());
		for (size_t i = 0; i < elements.size(); ++i) {
			Element* elem = elements[i];
			const auto& pts = elem->getPoints();
			unsigned int np = pts.size();
			if (np == 4) {
				neighbors[pts[0]].push_back(pts[1]); neighbors[pts[0]].push_back(pts[2]); neighbors[pts[0]].push_back(pts[3]);
				neighbors[pts[1]].push_back(pts[0]); neighbors[pts[1]].push_back(pts[2]); neighbors[pts[1]].push_back(pts[3]);
				neighbors[pts[2]].push_back(pts[0]); neighbors[pts[2]].push_back(pts[1]); neighbors[pts[2]].push_back(pts[3]);
				neighbors[pts[3]].push_back(pts[0]); neighbors[pts[3]].push_back(pts[1]); neighbors[pts[3]].push_back(pts[2]);
			} else if (np == 5) {
				neighbors[pts[0]].push_back(pts[1]); neighbors[pts[0]].push_back(pts[3]); neighbors[pts[0]].push_back(pts[4]);
				neighbors[pts[1]].push_back(pts[0]); neighbors[pts[1]].push_back(pts[2]); neighbors[pts[1]].push_back(pts[4]);
				neighbors[pts[2]].push_back(pts[1]); neighbors[pts[2]].push_back(pts[3]); neighbors[pts[2]].push_back(pts[4]);
				neighbors[pts[3]].push_back(pts[0]); neighbors[pts[3]].push_back(pts[2]); neighbors[pts[3]].push_back(pts[4]);
				neighbors[pts[4]].push_back(pts[0]); neighbors[pts[4]].push_back(pts[1]); neighbors[pts[4]].push_back(pts[2]); neighbors[pts[4]].push_back(pts[3]);
			} else if (np == 6) {
				neighbors[pts[0]].push_back(pts[1]); neighbors[pts[0]].push_back(pts[2]); neighbors[pts[0]].push_back(pts[3]);
				neighbors[pts[1]].push_back(pts[0]); neighbors[pts[1]].push_back(pts[2]); neighbors[pts[1]].push_back(pts[4]);
				neighbors[pts[2]].push_back(pts[0]); neighbors[pts[2]].push_back(pts[1]); neighbors[pts[2]].push_back(pts[5]);
				neighbors[pts[3]].push_back(pts[4]); neighbors[pts[3]].push_back(pts[5]); neighbors[pts[3]].push_back(pts[0]);
				neighbors[pts[4]].push_back(pts[3]); neighbors[pts[4]].push_back(pts[5]); neighbors[pts[4]].push_back(pts[1]);
				neighbors[pts[5]].push_back(pts[3]); neighbors[pts[5]].push_back(pts[4]); neighbors[pts[5]].push_back(pts[2]);
			} else if (np == 8) {
				neighbors[pts[0]].push_back(pts[1]); neighbors[pts[0]].push_back(pts[3]); neighbors[pts[0]].push_back(pts[4]);
				neighbors[pts[1]].push_back(pts[0]); neighbors[pts[1]].push_back(pts[2]); neighbors[pts[1]].push_back(pts[5]);
				neighbors[pts[2]].push_back(pts[1]); neighbors[pts[2]].push_back(pts[3]); neighbors[pts[2]].push_back(pts[6]);
				neighbors[pts[3]].push_back(pts[0]); neighbors[pts[3]].push_back(pts[2]); neighbors[pts[3]].push_back(pts[7]);
				neighbors[pts[4]].push_back(pts[5]); neighbors[pts[4]].push_back(pts[7]); neighbors[pts[4]].push_back(pts[0]);
				neighbors[pts[5]].push_back(pts[4]); neighbors[pts[5]].push_back(pts[6]); neighbors[pts[5]].push_back(pts[1]);
				neighbors[pts[6]].push_back(pts[5]); neighbors[pts[6]].push_back(pts[7]); neighbors[pts[6]].push_back(pts[2]);
				neighbors[pts[7]].push_back(pts[4]); neighbors[pts[7]].push_back(pts[6]); neighbors[pts[7]].push_back(pts[3]);
			}
		}

		std::vector<double> h_local(points.size(), 0.0);
		for (size_t i = 0; i < neighbors.size(); ++i) {
			std::sort(neighbors[i].begin(), neighbors[i].end());
			neighbors[i].erase(std::unique(neighbors[i].begin(), neighbors[i].end()), neighbors[i].end());
			if (neighbors[i].empty()) continue;
			double dist_sum = 0.0;
			for (size_t j = 0; j < neighbors[i].size(); ++j) {
				dist_sum += points[i].DistanceTo(points[neighbors[i][j]]);
			}
			h_local[i] = dist_sum / neighbors[i].size();
		}

		for (size_t i = 0; i < points.size(); ++i) {
			if (is_on_surface[i]) continue;
			double radius = d * h_local[i];
			if (radius <= 0.0) continue;
			double rx, ry, rz;
			do {
				rx = uni(rng) * radius;
				ry = uni(rng) * radius;
				rz = uni(rng) * radius;
			} while (rx * rx + ry * ry + rz * rz > radius * radius);
			points[i][0] += rx;
			points[i][1] += ry;
			points[i][2] += rz;
		}
	}

public:
	std::vector<Point3D> points;
	std::vector<Element*> elements;
	std::vector<std::vector<unsigned int>> node2elem_map;
};

int main(int argc, char **argv) {
	if (argc < 4) {
		std::cerr << "Usage: meshRandomizer <input.m3d> <output_base> <d> [seed] [keep_surface]\n";
		std::cerr << "  keep_surface: 1 to keep boundary points fixed, 0 otherwise (default: 0)\n";
		return 1;
	}

	std::string input = argv[1];
	std::string output_base = argv[2];
	double d = atof(argv[3]);
	unsigned int seed = 0;
	if (argc >= 5) seed = static_cast<unsigned int>(std::stoul(argv[4]));

	bool keep_surface = false;
	if (argc >= 6) keep_surface = (atoi(argv[5]) != 0);

	MeshRandomizer mr;
	if (!mr.readM3d(input)) {
		std::cerr << "Failed to read mesh: " << input << "\n";
		return 2;
	}

	mr.randomizeNodes(d, seed, keep_surface);

	if (!mr.writeM3d(output_base)) {
		std::cerr << "Failed to write M3D mesh: " << output_base << "\n";
		return 3;
	}

	if (!mr.writeVtk(output_base)) {
		std::cerr << "Failed to write VTK mesh: " << output_base << "\n";
		return 4;
	}

	std::cout << "Randomization applied (d=" << d << ", seed=" << seed << "). Outputs: " << output_base << ".m3d and " << output_base << ".vtk\n";
	return 0;
}

