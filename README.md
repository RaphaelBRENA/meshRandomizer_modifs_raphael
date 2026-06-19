# Bibliographie

## 1. Comment fonctionne les maillages 3D

### Concepts Clés
*   **Maillage Volumique** : Ne représente pas seulement la surface d'un objet, mais aussi l'intérieur de l'objet. Ils sont composés de formes géométriques 3D (tétraèdres, hexaèdres, pyramides, prismes).
*   **Qualité du Maillage** : Besoin d'éléments réguliers (par exemple un cube parfait). Si un élément est trop étiré, aplati ou inversé, les calculs physiques deviennent faux.

### Sources

* Projet de M1 sans IA, Claudio  
* [SimScale - Mesh Quality Guide](https://www.simscale.com/docs/simulation-setup/meshing/mesh-quality/) : Guide industriel.

---

## 2. Métriques de Qualité Géométrique (JENS, JENS2)
Métriques qui permettent de donner une note mathématique à chaque élément du maillage pour savoir s'il est correct.

### Concepts Clés
*   **Scaled Jacobian** : Mesure à quel point un élément est déformé ou tordu par rapport à sa forme idéale. Si le Jacobien est négatif, l'élément est inversé.
*   **JENS (Element Normalized Scaled Jacobian)** : Une métrique proposée par Claudio pour normaliser les calculs sur différents types d'éléments (tétraèdres, hexaèdres, etc.) afin de pouvoir les comparer équitablement (tous à 1).
*   **JENS2** : Une amélioration du JENS. Le JENS classique ne détectait pas bien si un élément était trop étiré sans être tordu. Le JENS2 corrige cela en intégrant la détection d'étirements (Aspect Ratio).

### Sources

*   **Papier Scientifique de Claudio** : ["Measuring geometrical quality of 3D linear element types."](https://www.researchgate.net/publication/355011980_Measuring_geometrical_quality_of_different_3D_linear_element_types)
*   **Rapport Projet de M1 sans IA**

---

## 3. Régularisation de Maillages (Classique vs IA)
Consiste à déplacer les sommets internes du maillage pour augmenter leur qualité globale, sans modifier la forme extérieure de l'objet.

### Concepts Clés à retenir
*   **Lissage Laplacien** : Technique classique très simple. On déplace chaque sommet vers le centre de gravité de ses voisins. Mais cela peut rétrécir l'objet, déformer les surfaces et parfois créer des éléments inversés.
*   **Smart Laplacian** : On gèle les sommets de surface (pour garder la forme globale) et on ne valide le déplacement d'un sommet interne que si sa nouvelle position améliore la qualité locale mesurée par une métrique (par exemple JENS2).
*   **Régularisation par IA (MLP)** : Entraîner un réseau de neurones (MLP) à prédire les coordonnées idéales d'un élément dégradé. Le réseau apprend à redresser les formes géométriques.

### Sources

*   **Rapport Projet M1 avec IA** : les étudiants y décrivent comment ils ont entraîné un MLP sous PyTorch pour régulariser des tétraèdres et des hexaèdres en appliquant un bruit gaussien synthétique, puis en effectuant l'inférence en C++ via LibTorch.
*   **Rapport Projet M1 sans IA** : Détaille les algorithmes classiques de *Laplacian Smooth* et *Smart Laplacian*.
*   **Code et Documentation du dépôt Github de Vincent** : présente l'optimisation de sommets par descente de gradient avec garantie d'absence d'inversion combinée avec des modèles de réseaux de neurones (MLP).

---

## 4. Graph Neural Networks (GNN)
Bien que le projet de M1 ne les utilise pas, on peut utiliser les Graph Neural Networks (GNN), car un maillage peut être vu comme un graphe.

### Concepts
*   **Geometric Deep Learning** : Branche de l'IA qui applique le deep learning à des données non structurées dans l'espace (comme les nuages de points 3D ou les maillages), contrairement aux images 2D classiques.
*   **Message Passing** : La méthode par laquelle les informations géométriques d'un sommet sont partagées avec ses voisins pour mettre à jour sa représentation.

### Sources

| Type de Source | Lien | Description |
| :--- | :--- | :--- |
| **Vidéo** | [Geometric Deep Learning on 3D Meshes](https://vertexaisearch.cloud.google.com/grounding-api-redirect/AUZIYQGoOuVG0KIcJ3it6jSgEQ4CssIS5bAWaomM1gpqKKEC8NKud5mY-Rjt9_TDHib08sPHvxHu35ipa-91_d3TecwjPePO62qKOnQ4djWre4dubsIzaW_aDlk7zg4Y5cswc4_-) | Cours sur l'extension du Deep Learning classique aux structures de maillage 3D. |
| **Vidéo** | [Learning Mesh-Based Simulation with Graph Networks](https://vertexaisearch.cloud.google.com/grounding-api-redirect/AUZIYQF1CvFt3y2ejEfZanrgng4QitXwmxS99MhVgbFOAzYyBrx9Ta-GwslNddAgH9U7Gmti__nPkGuYZsm3gIlT4EvCWPWFyplWzHFNIQQyXgIBdSDziecKv7CM7mSbYm8zhfhO) | Conférence expliquant comment utiliser les GNN pour accélérer et remplacer les simulations physiques traditionnelles sur maillage. |
| **Tutoriel Code** | [An Introduction to Deep Learning on Meshes](https://vertexaisearch.cloud.google.com/grounding-api-redirect/AUZIYQGRnEntg-B9mkrn7tJTJYJmAO1V4VaRqKk6C-MpYAegQ3Juwd-mwWQ2dg8Hd1spW6lCnfDxdEDJxRHA0KaGSF13iLJe6qY5UNcCrePFrQ6iYAE2ikWtsBIpXnxWAg9qUPAZ2_UwTqYbQ_WMr5vb) | Implémentations pratiques de réseaux de neurones appliqués aux maillages sous PyTorch. |

---

## 5. LLMs
Une autre façon d'appliquer l'IA aux maillages est d'utiliser les **Grands Modèles de Langage (LLMs)** pour écrire le code de modélisation et de génération de maillage dans les logiciels de simulation.

### Concepts Clés
*   **Ansys APDL / PyMAPDL** : Ansys est un logiciel de simulation industrielle. APDL est son langage de script. PyMAPDL est la bibliothèque Python permettant de contrôler Ansys.
*   **Framework PAMF** : Proposé dans l'article sur les LLM, ils utilisent un LLM (GPT-4o) pour générer des scripts APDL et les corriger de manière itérative en estimant l'erreur de simulation via un prédicteur d'erreur rapide (Gradient Boosting).

### Sources

*   **Article Scientifique LLM** : application des LLM appliqués à l'ingénierie mécanique.
*   **Documentation** : [Documentation PyMAPDL](https://mapdl.docs.pyansys.com/)

---

## 6. Modifications apportées au projet meshRandomizer
**[Lien dépot Git](https://github.com/RaphaelBRENA/meshRandomizer_modifs_raphael)**  
Plusieurs améliorations ont été implémentées directement dans le code source de `meshRandomizer`. 

### A. Export au format VTK
Afin de pouvoir visualiser directement les maillages de sortie dans ParaView, j'ai intégré la fonction de Claudio dans le code.
- **Fichier concerné** : [src/main.cpp]
- **Méthode ajoutée** dans la classe `MeshRandomizer` :
```cpp
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
```

### B. Déformation bidirectionnelle des coordonnées
Le code original appliquait une déformation seulement positive entre `[0, 1]`, ce que j'ai modifié pour appliquer une déformation entre [-1, 1].
- **Fichier concerné** : [src/main.cpp] (méthode `randomizeNodes`)
- **Modification** :
```cpp
		std::uniform_real_distribution<double> uni(-1.0, 1.0);
```

### C. Détection et préservation de la surface extérieure
Pour ne pas toucher aux sommets qui sont sur la surface, j'ai implémenté une détection des nœuds situés sur la surface extérieure.

Une face 2D appartient à la surface si elle est partagée par un seul élément volumique 3D du maillage. Si elle est partagée par deux éléments ou plus, elle est à l'intérieur.

La méthode `detectSurfaceNodes()` identifie la surface extérieure en répertoriant toutes les faces 2D des éléments du maillage et en comptant leurs occurrences. Les faces qui apparaissent une seule fois appartiennent à la surface de l'objet, et donc on marque tous leurs nœuds les composant comme étant sur la surface.  

Lors de la déformation dans `randomizeNodes()`, le code ignore les nœuds marqués sur la surface pour ne perturber que les sommets à l'intérieur du maillage.

1. **Méthode de détection `detectSurfaceNodes()`** :
- **Fichier concerné** : [src/main.cpp]
- **Code ajouté** :
```cpp
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
```

2. **Mise à jour de `randomizeNodes` pour ignorer les nœuds de surface** :
- **Fichier concerné** : [src/main.cpp]
- **Code modifié** : intégration du paramètre `keep_surface_fixed` (**+ modifications** pour avoir avoir un `d` relatif à la taille de l'élément ainsi que pour avoir une diagonale dans une sphère et non un cube).
```cpp
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
```

### D. Intégration dans le `main()`
Du code a été ajouté dans la fonction principale pour prendre en compte le nouveau paramètre `keep_surface` et pour appeler systématiquement l'export en VTK en plus de l'export M3D.
- **Fichier concerné** : [src/main.cpp]
- **Code modifié** :
```cpp
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
```

---

## 7. Génération de la base de données
**[Lien dépot Git](https://github.com/RaphaelBRENA/meshRandomizer_modifs_raphael)**  
Création d'un pipeline d'automatisation en Python pour générer une base de données de 1000 maillages déformés.

### A. Structure du dossier de la base de données
Tous les fichiers générés sont placés dans un sous-dossier :
- **Dossier de base** : [database/]
- **Entrées** : [database/inputs/] (contient les maillages sains)
- **Sorties** : [database/outputs/] (contient les dossiers de sortie par géométrie)

### B. Géométries d'entrée
Le pipeline utilise 4 géométries d'entrée, afin que l'on ait une diversité de formes et de nombres d'éléments :
1. **`femur_s4`** : ~9100 éléments.
2. **`cyl50_s4`** : ~9700 éléments.
3. **`hex_sphe_s3`** : ~570 éléments.
4. **`cortex_s3`** : ~800 éléments.

### C. Script Python d'orchestration
Le script Python automatise la génération en parallèle via des threads systèmes, et controle que tout s'est bien passé à la fin (**+ modifications** pour avoir plusieurs bandes de qualité, déplacement entre 5% et 45% de la longueur moyenne des arrêtes voisines du sommet).
- **Fichier du script** : [database/generate_database.py]
- **Code complet du script** :
```python
import os
import shutil
import subprocess
from concurrent.futures import ThreadPoolExecutor

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
PROJECT_ROOT = os.path.dirname(BASE_DIR)
EXE_PATH = os.path.join(PROJECT_ROOT, "meshRandomizer.exe")

INPUTS_DIR = os.path.join(BASE_DIR, "inputs")
OUTPUTS_DIR = os.path.join(BASE_DIR, "outputs")

MESH_CONFIGS = [
    "femur_s4",
    "cyl50_s4",
    "hex_sphe_s3",
    "cortex_s3",
]

NUM_PERTURBATIONS = 250

def setup_directories():
    print("Setting up directory structure...")
    os.makedirs(INPUTS_DIR, exist_ok=True)
    os.makedirs(OUTPUTS_DIR, exist_ok=True)
    for mesh_name in MESH_CONFIGS:
        os.makedirs(os.path.join(OUTPUTS_DIR, mesh_name), exist_ok=True)

def move_inputs():
    print("Checking and moving input meshes...")
    for mesh_name in MESH_CONFIGS:
        filename = f"{mesh_name}.m3d"
        src_path = os.path.join(PROJECT_ROOT, filename)
        dest_path = os.path.join(INPUTS_DIR, filename)
        
        if os.path.exists(dest_path):
            continue
            
        if os.path.exists(src_path):
            print(f"Moving {filename} to database/inputs/")
            shutil.move(src_path, dest_path)
            
            oct_filename = f"{mesh_name}.oct"
            oct_src = os.path.join(PROJECT_ROOT, oct_filename)
            oct_dest = os.path.join(INPUTS_DIR, oct_filename)
            if os.path.exists(oct_src):
                shutil.move(oct_src, oct_dest)
        else:
            print(f"Warning: Base mesh {filename} not found in root or inputs directory!")

def run_perturbation(args):
    mesh_name, idx = args
    d = 0.05 + 0.40 * (idx / (NUM_PERTURBATIONS - 1.0) if NUM_PERTURBATIONS > 1 else 0.0)
    input_file = os.path.join(INPUTS_DIR, f"{mesh_name}.m3d")
    output_base = os.path.join(OUTPUTS_DIR, mesh_name, f"{mesh_name}_p{idx}")
    seed = 1000 + idx
    keep_surface = "1"
    
    cmd = [
        EXE_PATH,
        input_file,
        output_base,
        str(d),
        str(seed),
        keep_surface
    ]
    
    try:
        subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, check=True)
        return True, mesh_name, idx, None
    except subprocess.CalledProcessError as e:
        error_msg = e.stderr.decode('utf-8', errors='ignore') or e.stdout.decode('utf-8', errors='ignore')
        return False, mesh_name, idx, f"Exit code {e.returncode}: {error_msg}"
    except Exception as e:
        return False, mesh_name, idx, str(e)

def generate_database():
    setup_directories()
    move_inputs()
    
    print("\nPreparing tasks...")
    tasks = []
    for mesh_name in MESH_CONFIGS:
        for idx in range(NUM_PERTURBATIONS):
            tasks.append((mesh_name, idx))
            
    total_tasks = len(tasks)
    print(f"Starting parallel generation of {total_tasks} perturbed meshes using ThreadPoolExecutor...")
    
    success_count = 0
    failure_count = 0
    failures = []
    
    max_workers = os.cpu_count() or 4
    
    with ThreadPoolExecutor(max_workers=max_workers) as executor:
        results = executor.map(run_perturbation, tasks)
        
        for success, mesh_name, idx, err in results:
            if success:
                success_count += 1
            else:
                failure_count += 1
                failures.append((mesh_name, idx, err))
            
            if (success_count + failure_count) % 100 == 0:
                print(f"Progress: {success_count + failure_count}/{total_tasks} completed...")

    print("\n" + "="*50)
    print("GENERATION SUMMARY")
    print("="*50)
    print(f"Total Tasks: {total_tasks}")
    print(f"Successful:  {success_count}")
    print(f"Failed:      {failure_count}")
    
    if failure_count > 0:
        print("\nFailures:")
        for mesh_name, idx, err in failures[:10]:
            print(f"  - {mesh_name} (index {idx}): {err}")
        if len(failures) > 10:
            print(f"  ... and {len(failures) - 10} more failures.")
        return False
    else:
        print("\nAll tasks completed successfully!")
        return True

def verify_files():
    print("\nRunning file verification check...")
    expected_files_per_mesh = NUM_PERTURBATIONS * 2
    total_expected = len(MESH_CONFIGS) * expected_files_per_mesh
    actual_count = 0
    missing_files = []
    
    for mesh_name in MESH_CONFIGS:
        mesh_dir = os.path.join(OUTPUTS_DIR, mesh_name)
        file_list = os.listdir(mesh_dir)
        
        for idx in range(NUM_PERTURBATIONS):
            m3d_file = f"{mesh_name}_p{idx}.m3d"
            vtk_file = f"{mesh_name}_p{idx}.vtk"
            
            if m3d_file not in file_list:
                missing_files.append(os.path.join(mesh_name, m3d_file))
            else:
                if os.path.getsize(os.path.join(mesh_dir, m3d_file)) == 0:
                    missing_files.append(os.path.join(mesh_name, f"{m3d_file} (empty)"))
                else:
                    actual_count += 1
                    
            if vtk_file not in file_list:
                missing_files.append(os.path.join(mesh_name, vtk_file))
            else:
                if os.path.getsize(os.path.join(mesh_dir, vtk_file)) == 0:
                    missing_files.append(os.path.join(mesh_name, f"{vtk_file} (empty)"))
                else:
                    actual_count += 1

    print(f"Expected files: {total_expected}")
    print(f"Found non-empty files: {actual_count}")
    
    if missing_files:
        print(f"Verification FAILED. {len(missing_files)} missing or empty files:")
        for f in missing_files[:10]:
            print(f"  - {f}")
        return False
    else:
        print("Verification PASSED.")
        return True

if __name__ == "__main__":
    success = generate_database()
    if success:
        verify_files()
```

### D. Résultats

L'exécution du script de génération a permis de créer la base de données suivante :
- **1000 fichiers de maillages volumiques déformés au format M3D** (250 par géométrie).
- **1000 fichiers d'exportation au format VTK** correspondants pour la visualisation dans ParaView.
- **Nombre total de fichiers générés** : 2000 fichiers.

---

## 8. Analyse Statistique avec le JEANS

Analyse dans le fichier PDF `analysis\report_statistical_analysis.pdf`.