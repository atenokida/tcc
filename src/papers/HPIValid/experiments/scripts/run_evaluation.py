import subprocess
import os


def report_error(result):
    if result.returncode != 0:
        print("error:")
        print(result.stderr)


def run_R_script(name):
    print("Running " + name)
    r = subprocess.run("Rscript " + name, shell=True, capture_output=True)
    report_error(r)


os.chdir("evaluation/")
print("Run all R scripts")
run_R_script("efficiency.R")
run_R_script("impl_detail.R")
run_R_script("run_time_breakdown.R")
run_R_script("run_time_table.R")
run_R_script("sampling.R")
run_R_script("scaling_breakdown.R")
run_R_script("scaling_isolet.R")

os.chdir("output/latex/")
print("Run LaTeX")
r = subprocess.run("pdflatex evaluation.tex", shell=True, capture_output=True)
r = subprocess.run("pdflatex evaluation.tex", shell=True, capture_output=True)
report_error(r)
