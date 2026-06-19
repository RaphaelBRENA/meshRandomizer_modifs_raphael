#include <vector>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <algorithm>
#include "../Point3D.h"
#include "../Element.h"
#include "../Hexahedron.h"
#include "../Prism.h"
#include "../Pyramid.h"
#include "../Tetrahedron.h"

using namespace std;
using Clobscode::Point3D;

class QualityEvaluator {
public:
    QualityEvaluator() {}
    ~QualityEvaluator() {
        for (size_t i = 0; i < elements.size(); ++i) {
            if (elements[i]) delete elements[i];
        }
    }

    bool readM3d(const string &name) {
        char word[256];
        int pcant = 0, ecant = 0;
        bool mvm = false;

        FILE *file = fopen(name.c_str(), "r");
        if (file == NULL) {
            return false;
        }

        while (true) {
            if (fscanf(file, "%s", word) == EOF) {
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

        if (fscanf(file, "%i", &pcant) == EOF) {
            fclose(file);
            return false;
        }
        if (pcant <= 0) {
            fclose(file);
            return false;
        }

        if (mvm) {
            if (fscanf(file, "%i", &ecant) == EOF) {
                fclose(file);
                return false;
            }
            if (ecant <= 0) {
                fclose(file);
                return false;
            }
        }

        points.clear();
        points.reserve(pcant);

        for (int i = 0; i < pcant; i++) {
            if (!mvm) {
                if (fscanf(file, "%s", word) == EOF) {
                    fclose(file);
                    return false;
                }
            }
            double x, y, z;
            if (fscanf(file, "%s", word) == EOF) {
                fclose(file);
                return false;
            }
            x = atof(word);
            if (fscanf(file, "%s", word) == EOF) {
                fclose(file);
                return false;
            }
            y = atof(word);
            if (fscanf(file, "%s", word) == EOF) {
                fclose(file);
                return false;
            }
            z = atof(word);
            Point3D p(x, y, z);
            points.push_back(p);
        }

        if (!mvm) {
            while (true) {
                if (fscanf(file, "%s", word) == EOF) {
                    fclose(file);
                    return false;
                }
                if (!strcmp(word, "ARRAY1<STRING>]\0")) {
                    if (fscanf(file, "%i", &ecant) == EOF) {
                        fclose(file);
                        return false;
                    }
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

        for (int i = 0; i < ecant; i++) {
            if (fscanf(file, "%s", word) == EOF) {
                fclose(file);
                return false;
            }

            if (!strcmp(word, "H\0")) {
                vector<int> element(8, 0);
                for (unsigned int j = 0; j < 8; j++) {
                    if (fscanf(file, "%i", &element[j]) == EOF) {
                        fclose(file);
                        return false;
                    }
                }
                elements.push_back(new Hexahedron(element));
            }
            else if (!strcmp(word, "R\0")) {
                vector<int> element(6, 0);
                for (unsigned int j = 0; j < 6; j++) {
                    if (fscanf(file, "%i", &element[j]) == EOF) {
                        fclose(file);
                        return false;
                    }
                }
                elements.push_back(new Prism(element));
            }
            else if (!strcmp(word, "P\0")) {
                vector<int> element(5, 0);
                for (unsigned int j = 0; j < 5; j++) {
                    if (fscanf(file, "%i", &element[j]) == EOF) {
                        fclose(file);
                        return false;
                    }
                }
                elements.push_back(new Pyramid(element));
            }
            else if (!strcmp(word, "T\0")) {
                vector<int> element(4, 0);
                for (unsigned int j = 0; j < 4; j++) {
                    if (fscanf(file, "%i", &element[j]) == EOF) {
                        fclose(file);
                        return false;
                    }
                }
                elements.push_back(new Tetrahedron(element));
            }
            else {
                fclose(file);
                return false;
            }

            if (!mvm) {
                for (unsigned int j = 0; j < 3; j++) {
                    if (fscanf(file, "%s", word) == EOF) {
                        fclose(file);
                        return false;
                    }
                }
            }
        }
        fclose(file);
        return true;
    }

    void evaluateAndPrint() {
        cout << "element_idx,element_type,jens,jeans\n";
        for (size_t i = 0; i < elements.size(); ++i) {
            Element* elem = elements[i];
            double jens = elem->getElementJENS(points);
            double jeans = elem->getElementJEANS(points);
            char type_char = 'U';
            size_t np = elem->getPoints().size();
            if (np == 4) type_char = 'T';
            else if (np == 5) type_char = 'P';
            else if (np == 6) type_char = 'R';
            else if (np == 8) type_char = 'H';
            cout << i << "," << type_char << "," << jens << "," << jeans << "\n";
        }
    }

private:
    vector<Point3D> points;
    vector<Element*> elements;
};

int main(int argc, char **argv) {
    if (argc < 2) {
        return 1;
    }
    string input = argv[1];
    QualityEvaluator qe;
    if (!qe.readM3d(input)) {
        return 2;
    }
    qe.evaluateAndPrint();
    return 0;
}
