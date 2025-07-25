"""
Framework for HPIValid experiments including HyUCC comparison.

This fills out the gaps left by the general experiments framework to
fit the needs of the HPIValid experiments.  This consists of the
following four parts:

    - Some general settings like the location of the data directory
      and global settings for the algorithms.

    - Implementations for the command and result functions
      (C{command_fun_*} and C{result_fun_*}).

    - Wrapper functions for running the experiments
      (L{run_experiment_hpiv} and L{run_experiment_hyucc}).

    - The function L{subtable_runs} as a helper to create subtables
      before the actual runs.

@author: Thomas Blaesius
"""

import re
import os
from pathlib import Path
from os import path
from experiments import flat_runs, run_experiment


###############################################################################
# general settings

#: The directory containing the source databases.
data_dir = "data_raw/"

#: The place where subtables are cached.
data_dir_subtbl = "data_subtables/"

#: Output for the collected stats.
out_dir = "results/"

#: Global options for HPIValid.
glob_opt_hpiv = {"no-output": True,
                 "subtable-dir": data_dir_subtbl}

#: Global options for HyUCC.
glob_opt_hyucc = {"timeout": 3600,
                  "memory-limit": "25g"}

#: If true, only dry runs are performed.
dry_run = False

#: File holding the java safepoint log
safepoint_log = "safepoint.log"


###############################################################################
# command and result functions

def command_fun_hpiv(run):
    """The HPIValid command for a given run.

    To be used as the command_fun in L{run_experiment}.

    @param run: A single run (as, e.g., returned by L{flat_runs}).

    @return: The command line command as string.
    """
    opt_string = " -i " + data_dir + run["instance"] + ".csv"
    for key, value in run["options"].items():
        if key in ["output-dir", "no-output", "rows", "cols",
                   "subtable-dir", "do-not-run", "sample-exponent",
                   "seed", "timeout", "header", "no-copy-PLIs",
                   "no-tiebreaker-heuristic"]:
            if type(value) == str:
                opt_string += " --" + key + " " + value
            elif type(value) == bool:
                opt_string += " --" + key if value else ""
            else:
                opt_string += " --" + key + " " + str(value)
    return ("/usr/bin/time -f '%M' ../release/HPIValid --output-dir " + out_dir
            + opt_string)


def result_fun_hpiv(run, result):
    """Parser for the result of a HPIValid run.

    To be used as result_fun in L{run_experiment}.

    @param run: The run that was performed.

    @param result: The result of the run as returned by
        C{subprocess.run()}.

    @return: The result stats formatted as line of a csv file.
    """
    if result.returncode != 0:
        # this should not happen -> report error
        print(result.stderr)
        return "error for run: " + str(run)

    if "header" in run["options"] and run["options"]["header"]:
        # the run was just about printing the header
        return result.stdout.strip() + ",memory"
    else:
        # normal run -> algo output + memory measurement
        return result.stdout.strip() + "," + result.stderr.strip()


def command_fun_hyucc(run):
    """The HyUCC command for a given run.

    To be used as the command_fun in L{run_experiment}.

    @param run: A single run (as, e.g., returned by L{flat_runs}).

    @return: The command line command as string.
    """
    rows = run["options"]["rows"] if "rows" in run["options"] else 0
    cols = run["options"]["cols"] if "cols" in run["options"] else 0
    if rows == 0 and cols == 0:
        name = run["instance"]
        path = data_dir + name + ".csv"
    else:
        name = run["instance"] + "/r" + str(rows) + "_c" + str(cols)
        path = (data_dir_subtbl + name + ".csv")

    timeout = str(run["options"]["timeout"])
    memory = run["options"]["memory-limit"]
    return ("/usr/bin/time -f '%M' timeout --preserve-status " + timeout +
            " java -Xmx" + memory +
            " -Xlog:disable -Xlog:safepoint=info:" + safepoint_log +
            " -server -XX:CICompilerCount=4" +
            " -XX:+UseParallelGC -XX:ParallelGCThreads=8 -XX:ConcGCThreads=4" +
            " -jar HyUCC/HyUCCTestRunner-1.2-SNAPSHOT.jar " + path +
            " , false")


def result_fun_hyucc(run, result):
    """Parser for the result of a HyUCC run.

    To be used as result_fun in L{run_experiment}.

    @param run: The run that was performed.

    @param result: The result of the run as returned by
        C{subprocess.run()}.

    @return: The result stats formatted as line of a csv file.
    """
    instance = run["instance"]
    rows = (run["options"]["rows"] if "rows" in run["options"] and
            run["options"]["rows"] != 0 else int(instance.split("_")[-2][1:]))
    cols = (run["options"]["cols"] if "cols" in run["options"] and
            run["options"]["cols"] != 0 else int(instance.split("_")[-1][1:]))

    time_sp = total_safepoint_time()
    if path.exists(safepoint_log):
        os.remove(safepoint_log)

    if result.returncode == 0:
        # everything ok
        time = re.search(r"(?<=Time: )\d*", result.stdout).group(0)
        uccs = re.search(r"(?<=\.\.\. done! \()\d*", result.stdout).group(0)
        mem = result.stderr.strip()
        return ",".join([instance, str(rows), str(cols),
                         time, mem, uccs, str(time_sp)])
    elif result.returncode == 1:
        # memory limit
        return ",".join([instance, str(rows), str(cols),
                         "0", "0", "0", str(time_sp)])
    elif result.returncode == 143:
        # time limit
        mem = result.stderr.split('\n')[1]
        timeout = str(run["options"]["timeout"] * 1000)
        return ",".join([instance, str(rows), str(cols),
                         timeout, mem, "0", str(time_sp)])
    else:
        # this should not happen
        print(result.stderr)
        return "error for run: " + str(run)


def result_fun_subtable(run, result):
    """Parser for a HPIValid run that only created a subtable.

    To be used as result_fun in L{run_experiment}.

    @param run: The run that was performed.

    @param result: The result of the run as returned by
        C{subprocess.run()}.

    @return: A string containing the dataset and the size of the
        resulting subtable.
    """
    return ("subtable: " + run["instance"] + "(r = " +
            str(run["options"]["rows"]) + ", c = " +
            str(run["options"]["cols"]) + ")")


def result_fun_identity(run, result):
    """Result function just returning the output.
    """
    return result.stdout


###############################################################################
# helper function parsing the java safepoint log
def total_safepoint_time():
    """Parse the safepoint log to get total time."""
    if not path.exists(safepoint_log):
        return 0
    log = open(safepoint_log, "r")
    lines = log.readlines()
    times = [int(line[line.find("Total:")+7:-4]) for line in lines]
    return sum(times)


###############################################################################
# wrapper functions for running the experiments

def run_experiment_hpiv(experiment, filename):
    """Run experiments for HPIValid

    Use L{flat_runs} and L{run_experiment} to run a given experiment
    with the default settings for HPIValid and append output to the
    given file.

    @param experiment: An experiment specification; see L{flat_runs}.

    @param filename: Filename for the output.  The file will be
        created in L{out_dir}.
    """
    Path(out_dir).mkdir(parents=True, exist_ok=True)
    with open(out_dir + filename, "a") as out:
        run_experiment(flat_runs(experiment, glob_opt_hpiv),
                       command_fun_hpiv, result_fun_hpiv, out=out,
                       dry_run=dry_run)


def print_header_hyucc(out):
    """Helper function printing the HyUCC header."""
    print("dataset,rows,cols,hyucc_t_total_ms,hyucc_memory,hyucc_ucc_count," +
          "hyucc_t_safepoint_ns",
          file=out, flush=True)


def run_experiment_hyucc(experiment, filename, print_header=True):
    """Run experiments for HyUCC

    Use L{flat_runs} and L{run_experiment} to run a given experiment
    with the default settings for HPIValid and append output to the
    given file.

    @param experiment: An experiment specification; see L{flat_runs}.

    @param filename: Filename for the output.  The file will be
        created in L{out_dir}.
    """
    Path(out_dir).mkdir(parents=True, exist_ok=True)
    with open(out_dir + filename, "a") as out:
        if print_header:
            print_header_hyucc(out)
        run_experiment(flat_runs(experiment, glob_opt_hyucc),
                       command_fun_hyucc, result_fun_hyucc, out=out,
                       dry_run=dry_run)


###############################################################################
# helper for creating the subtables

def subtable_runs(runs):
    """Create runs for just creating subtables from actual runs.

    Some of the runs use subtables as inputs.  HPIValid could create
    them on the fly, which, however, would interfere with the memory
    measurements.  Moreover, for HyUCC the subtables have to be
    created beforehand.  This function takes the actual HPIValid
    and/or HyUCC runs and returns a list of runs for just creating the
    subtables.

    @param runs: A list of runs as, e.g., returned by L{flat_runs}.

    @return: A list of runs that just create the necessary subtables
        to afterwards perform the runs given as input.
    """
    res = []
    for run in runs:
        r = (run["instance"],
             run["options"]["rows"] if "rows" in run["options"] else 0,
             run["options"]["cols"] if "cols" in run["options"] else 0)

        if r[1] != 0 or r[2] != 0:
            res.append(r)

    res = [{"instance": r[0],
            "options": {"rows": r[1], "cols": r[2], "do-not-run": True,
                        "no-output": True, "subtable-dir": data_dir_subtbl}}
           for r in sorted(list(set(res)))]
    return res
