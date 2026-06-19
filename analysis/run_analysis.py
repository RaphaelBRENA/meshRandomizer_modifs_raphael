import os
import csv
import subprocess
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
from concurrent.futures import ThreadPoolExecutor

os.makedirs("analysis/plots", exist_ok=True)
os.makedirs("analysis/raw_details", exist_ok=True)

base_dir = "database/outputs"
meshes = ["femur_s4", "cyl50_s4", "hex_sphe_s3", "cortex_s3"]
num_perturbations = 250
exe_path = os.path.abspath("analysis/evaluate_quality.exe")

def process_single_mesh(args):
    mesh_name, idx = args
    d = 0.05 + 0.40 * (idx / 249.0)
    input_file = os.path.join(base_dir, mesh_name, f"{mesh_name}_p{idx}.m3d")
    
    if not os.path.exists(input_file):
        return None
        
    res = subprocess.run([exe_path, input_file], capture_output=True, text=True)
    if res.returncode != 0:
        return None
        
    lines = res.stdout.strip().split("\n")
    if len(lines) <= 1:
        return None
        
    reader = csv.reader(lines)
    header = next(reader)
    
    elements_data = []
    for row in reader:
        elem_idx = int(row[0])
        elem_type = row[1]
        jens = float(row[2])
        jeans = float(row[3])
        elements_data.append((elem_idx, elem_type, jens, jeans))
        
    df = pd.DataFrame(elements_data, columns=["element_idx", "element_type", "jens", "jeans"])
    
    is_detail = idx in [0, 125, 249]
    if is_detail:
        detail_path = f"analysis/raw_details/detail_{mesh_name}_p{idx}.csv"
        df.to_csv(detail_path, index=False)
        
    num_elements = len(df)
    min_jens = df["jens"].min()
    avg_jens = df["jens"].mean()
    min_jeans = df["jeans"].min()
    avg_jeans = df["jeans"].mean()
    num_inverted_jens = int((df["jens"] < 0).sum())
    num_inverted_jeans = int((df["jeans"] < 0).sum())
    
    type_stats = {}
    for t in ["T", "P", "R", "H"]:
        sub = df[df["element_type"] == t]
        if len(sub) > 0:
            type_stats[f"avg_jeans_{t}"] = sub["jeans"].mean()
            type_stats[f"count_{t}"] = len(sub)
            type_stats[f"inverted_{t}"] = int((sub["jeans"] < 0).sum())
        else:
            type_stats[f"avg_jeans_{t}"] = np.nan
            type_stats[f"count_{t}"] = 0
            type_stats[f"inverted_{t}"] = 0
            
    res_row = {
        "mesh_name": mesh_name,
        "idx": idx,
        "d": d,
        "min_jens": min_jens,
        "avg_jens": avg_jens,
        "min_jeans": min_jeans,
        "avg_jeans": avg_jeans,
        "num_inverted_jens": num_inverted_jens,
        "num_inverted_jeans": num_inverted_jeans,
        "num_elements": num_elements,
        **type_stats
    }
    return res_row

tasks = []
for mesh in meshes:
    for idx in range(num_perturbations):
        tasks.append((mesh, idx))

results = []
max_workers = os.cpu_count() or 4
with ThreadPoolExecutor(max_workers=max_workers) as executor:
    for r in executor.map(process_single_mesh, tasks):
        if r is not None:
            results.append(r)

agg_df = pd.DataFrame(results)
agg_df.to_csv("analysis/mesh_quality_metrics.csv", index=False)

sns.set_theme(style="darkgrid")
plt.rcParams.update({"font.size": 11})

fig, axes = plt.subplots(1, 2, figsize=(14, 6))
for mesh in meshes:
    sub = agg_df[agg_df["mesh_name"] == mesh].sort_values("d")
    axes[0].plot(sub["d"], sub["avg_jeans"], label=mesh, linewidth=2.5)
axes[0].set_title("Qualite Moyenne JEANS vs d")
axes[0].set_xlabel("Perturbation d")
axes[0].set_ylabel("Qualite Moyenne")
axes[0].set_ylim(-0.1, 1.05)
axes[0].legend()

for mesh in meshes:
    sub = agg_df[agg_df["mesh_name"] == mesh].sort_values("d")
    axes[1].plot(sub["d"], sub["min_jeans"], label=mesh, linewidth=2.5)
axes[1].set_title("Qualite Minimale JEANS vs d")
axes[1].set_xlabel("Perturbation d")
axes[1].set_ylabel("Qualite Minimale")
axes[1].set_ylim(-1.05, 1.05)
axes[1].legend()
plt.tight_layout()
plt.savefig("analysis/plots/plot1_quality_vs_d.png", dpi=200)
plt.close()

plt.figure(figsize=(9, 6))
for mesh in meshes:
    sub = agg_df[agg_df["mesh_name"] == mesh].sort_values("d")
    pct_inverted = (sub["num_inverted_jeans"] / sub["num_elements"]) * 100.0
    plt.plot(sub["d"], pct_inverted, label=mesh, linewidth=2.5)
plt.title("Taux d'Elements Inverses (JEANS < 0) vs d")
plt.xlabel("Perturbation d")
plt.ylabel("Elements Inverses (%)")
plt.ylim(-0.5, 8.0)
plt.legend()
plt.tight_layout()
plt.savefig("analysis/plots/plot2_inversions_vs_d.png", dpi=200)
plt.close()

violin_data = []
for mesh in meshes:
    for idx in [0, 125, 249]:
        path = f"analysis/raw_details/detail_{mesh}_p{idx}.csv"
        if os.path.exists(path):
            df = pd.read_csv(path)
            d_val = 0.05 + 0.40 * (idx / 249.0)
            df["mesh_name"] = mesh
            df["perturbation"] = f"d={d_val:.2f}"
            violin_data.append(df)
            
if violin_data:
    vdf = pd.concat(violin_data, ignore_index=True)
    plt.figure(figsize=(12, 6))
    sns.violinplot(data=vdf, x="mesh_name", y="jeans", hue="perturbation", split=False, inner="quart")
    plt.title("Distribution de la Qualite JEANS par Niveau de Perturbation")
    plt.xlabel("Geometrie de Maillage")
    plt.ylabel("Qualite JEANS")
    plt.ylim(-1.05, 1.05)
    plt.legend(title="Niveau de Perturbation")
    plt.tight_layout()
    plt.savefig("analysis/plots/plot3_violin_distribution.png", dpi=200)
    plt.close()

fig, axes = plt.subplots(2, 2, figsize=(14, 10))
axes = axes.flatten()
for i, mesh in enumerate(meshes):
    sub = agg_df[agg_df["mesh_name"] == mesh].sort_values("d").copy()
    
    excellent = []
    good = []
    poor = []
    inverted = []
    
    for idx in sub["idx"]:
        path = f"analysis/raw_details/detail_{mesh}_p{idx}.csv"
        if os.path.exists(path):
            df = pd.read_csv(path)
            total = len(df)
            excellent.append((df["jeans"] >= 0.8).sum() / total * 100.0)
            good.append(((df["jeans"] >= 0.4) & (df["jeans"] < 0.8)).sum() / total * 100.0)
            poor.append(((df["jeans"] >= 0.0) & (df["jeans"] < 0.4)).sum() / total * 100.0)
            inverted.append((df["jeans"] < 0.0).sum() / total * 100.0)
        else:
            excellent.append(np.nan)
            good.append(np.nan)
            poor.append(np.nan)
            inverted.append(np.nan)
            
    sub["Excellent"] = excellent
    sub["Bon"] = good
    sub["Mediocre"] = poor
    sub["Inverse"] = inverted
    sub = sub.dropna(subset=["Excellent"])
    
    axes[i].stackplot(
        sub["d"],
        sub["Inverse"],
        sub["Mediocre"],
        sub["Bon"],
        sub["Excellent"],
        labels=["Inverse (< 0)", "Mediocre [0, 0.4)", "Bon [0.4, 0.8)", "Excellent [0.8, 1.0]"],
        colors=["#e74c3c", "#f39c12", "#3498db", "#2ecc71"],
        alpha=0.85
    )
    axes[i].set_title(f"Bandes de Qualite: {mesh}")
    axes[i].set_xlabel("Perturbation d")
    axes[i].set_ylabel("Proportion des Elements (%)")
    axes[i].set_xlim(0.05, 0.45)
    axes[i].set_ylim(0, 100)
    if i == 0:
        axes[i].legend(loc="lower left")
plt.tight_layout()
plt.savefig("analysis/plots/plot4_quality_bands.png", dpi=200)
plt.close()

jens_jeans_data = []
for mesh in meshes:
    path = f"analysis/raw_details/detail_{mesh}_p249.csv"
    if os.path.exists(path):
        df = pd.read_csv(path)
        df["mesh_name"] = mesh
        jens_jeans_data.append(df)
        
if jens_jeans_data:
    jjdf = pd.concat(jens_jeans_data, ignore_index=True)
    plt.figure(figsize=(8, 8))
    sns.scatterplot(data=jjdf, x="jens", y="jeans", alpha=0.3, s=15, hue="mesh_name", palette="deep")
    plt.plot([-1.1, 1.1], [-1.1, 1.1], color="red", linestyle="--", linewidth=1.5, label="y = x")
    plt.title("Comparaison JENS vs JEANS à d = 0.45")
    plt.xlabel("Qualite JENS (JENS 1)")
    plt.ylabel("Qualite JEANS (JENS 2)")
    plt.xlim(-1.05, 1.05)
    plt.ylim(-1.05, 1.05)
    plt.legend()
    plt.tight_layout()
    plt.savefig("analysis/plots/plot5_jens_vs_jeans.png", dpi=200)
    plt.close()

fig, axes = plt.subplots(1, 2, figsize=(14, 6))
mixed_meshes = ["hex_sphe_s3", "cortex_s3"]
for idx, mesh in enumerate(mixed_meshes):
    sub = agg_df[agg_df["mesh_name"] == mesh].sort_values("d")
    for t in ["T", "P", "R", "H"]:
        col_name = f"avg_jeans_{t}"
        if col_name in sub.columns and not sub[col_name].isna().all():
            axes[idx].plot(sub["d"], sub[col_name], label=f"Type {t}", linewidth=2.5)
    axes[idx].set_title(f"Qualite JEANS par Type d'Element: {mesh}")
    axes[idx].set_xlabel("Perturbation d")
    axes[idx].set_ylabel("Qualite Moyenne")
    axes[idx].set_ylim(-0.1, 1.05)
    axes[idx].legend()
plt.tight_layout()
plt.savefig("analysis/plots/plot6_quality_by_element_type.png", dpi=200)
plt.close()
