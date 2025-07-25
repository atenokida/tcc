"""
Main experiments for HPIValid restricted to publicly available datasets.

@author: Thomas Blaesius
"""

from experiments import flat_runs, run_experiment
import experiments_hpiv as eh

###############################################################################
# HPIValid experiments

#: General run time experiments for HPIValid.
exp_hpiv_run_time = {
    "instances": [
        # just printing the header
        ["-", {"header": True, "iterations": 1}],
        # low run times (< 1min)
        # "CE4HI01_r1678782_c65", # not available
        "Hospital_r114920_c15",
        # "ILOA_r20000000_c48", # not available
        "PDBX_POLY_SEQ_SCHEME_r17305799_c13",
        "SG_BIOENTRY_DBXREF_ASSOC_r1854789_c3",
        "SG_BIOENTRY_QUALIFIER_ASSOC_r1821546_c4",
        "SG_BIOENTRY_REF_ASSOC_r358337_c5",
        "SG_BIOENTRY_r184292_c9",
        "SG_BIOSEQUENCE_r184292_c6",
        "SG_DBXREF_r618244_c4",
        "SG_LOCATION_r1020335_c8",
        "SG_REFERENCE_r128816_c6",
        "SG_SEQFEATURE_QUALIFIER_ASSOC_r825116_c4",
        "SG_SEQFEATURE_r1020335_c6",
        "SG_TAXON_NAME_r105826_c3",
        "SPStock_r122497_c7",
        "Tax_r1000001_c15",
        "WDC_age_r10_c3",
        "WDC_astrology_r13_c7",
        "WDC_astronomical_r12_c6",
        "WDC_game_r9_c5",
        "WDC_kepler_r10_c4",
        "WDC_planetz_r11_c5",
        "WDC_satellites_r174_c8",
        "WDC_science_r10_c3",
        "WDC_symbols_r64_c3",
        # "ZBC00DT_COCM_r3175806_c35", # not available
        "amalgam1_denormalized_r51_c87",
        "census_r196295_c42",
        "chess_r28056_c7",
        "ditag_feature_r3960124_c13",
        "echocardiogram_r132_c13",
        "entytysrcgen_r26139_c46",
        "fd-reduced-30_r250001_c30",
        "flight_r1001_c109",
        "hepatitis_r155_c20",
        "horse_r300_c29",
        "iris_r147_c5",
        ["isolet_r7798_c618", {"cols": 200}],
        "letter_r18668_c17",
        "musicbrainz_denormalized_r79569_c100",
        ["ncvoter_allc_r7503554_c94", {"rows": [100000, 1500000]}],
        "ncvoter_r8060060_c19",
        "nursery_r12960_c9",
        "plista_r996_c63",
        "struct_sheet_range_r664128_c32",
        "t_biocase_gathering_agent_r72738_c18",
        "t_biocase_gathering_r90992_c35",
        "t_biocase_gathering_namedareas_r137711_c11",
        "t_biocase_gathering_sitecoordinates_r91349_c25",
        "t_biocase_identification_r91800_c38",
        "t_biocase_identification_highertaxon_r562959_c3",
        "t_biocase_measurementsorfacts_r3113_c24",
        "t_biocase_metadata_r4_c56",
        "t_biocase_multimediaobject_r18785_c15",
        "t_biocase_preparation_r81780_c21",
        "t_biocase_specimenunit_mark_r8976_c12",
        "t_biocase_unit_r91349_c14",
        "uniprot_r539166_c223",
        ["uniprot_r539166_c223", {"rows": 1001, "cols": 120}],
        # higher run times (> 1min)
        # "VTTS_r13042057_c75", # not available
        "lineitem_r6001215_c16",
        "ncvoter_allc_r7503554_c94",
        "tpch_denormalized_r6001216_c52",
        # timeout (1h) -> only 1 iteration
        ["isolet_r7798_c618", {"iterations": 1}],
        ["uniprot_r539166_c223", {"rows": 1001, "iterations": 1}]
    ],
    "options": {
        "iterations": 5
    }
}

#: Optimization experiments for HPIValid.
exp_hpiv_optimizations = {
    "instances": [
        # just printing the header
        ["-", {"header": True, "iterations": 1,
               "no-copy-PLIs": False, "no-tiebreaker-heuristic": False}],
        # actual instances
        "lineitem_r6001215_c16",
        "ncvoter_r8060060_c19",
        ["tpch_denormalized_r6001216_c52", {"rows": 500000}]
    ],
    "options": {
        "iterations": 25,
        "no-copy-PLIs": [True, False],
        "no-tiebreaker-heuristic": [True, False]
    }
}

#: Scaling experiments for HPIValid.
exp_hpiv_scaling = {
    "instances": [
        # just printing the header
        ["-", {"header": True, "iterations": 1}],
        # isolet col scaling with only 3 iterations and 900s timeout
        ["isolet_r7798_c618",
         {"cols": [40,  80, 120, 160, 200, 240, 280, 320, 360, 400],
          "iterations": 3,
          "timeout": 900}],
        # col scaling for uniprot, and ncvoter_allc (with all and 100k rows)
        ["uniprot_r539166_c223",
         {"cols": [20, 40, 60, 80, 100, 120, 140, 160, 180, 200, 220]}],
        ["ncvoter_allc_r7503554_c94",
         {"rows": [0, 100000],
          "cols": [10, 20, 30, 40, 50, 60, 70, 80, 90]}],
        # row scaling for lineitem, VTTS, and ncvoter_allc
        ["lineitem_r6001215_c16",
         {"rows": [500000, 1000000, 1500000, 2000000, 2500000, 3000000,
                   3500000, 4000000, 4500000, 5000000, 5500000, 6000000]}],
        # ["VTTS_r13042057_c75", # not available
        #  {"rows": [1000000, 2000000, 3000000, 4000000, 5000000,
        #            6000000, 7000000, 8000000, 9000000, 10000000,
        #            11000000, 12000000, 13000000]}],
        ["ncvoter_allc_r7503554_c94",
         {"rows": [500000, 1000000, 1500000, 2000000, 2500000, 3000000,
                   3500000, 4000000, 4500000, 5000000, 5500000, 6000000,
                   6500000, 7000000]}]
    ],
    "options": {
        "iterations": 15
    }
}

#: Sampling experiments for HPIValid.
exp_hpiv_sampling = {
    "instances": [
        # just printing the header
        ["-", {"header": True, "iterations": 1, "sample-exponent": 0.3}],
        # actual instances
        "census_r196295_c42",
        "hepatitis_r155_c20",
        "struct_sheet_range_r664128_c32",
        ["ncvoter_allc_r7503554_c94", {"rows": 1000000}]
    ],
    "options": {
        "iterations": 5,
        "sample-exponent": [0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6]
    }
}

###############################################################################
# HyUCC experiments

#: General run time experiments for HyUCC.
exp_hyucc_run_time = {
    "instances": [
        # "CE4HI01_r1678782_c65", # not available
        "Hospital_r114920_c15",
        # "ILOA_r20000000_c48", # not available
        "PDBX_POLY_SEQ_SCHEME_r17305799_c13",
        "SG_BIOENTRY_DBXREF_ASSOC_r1854789_c3",
        "SG_BIOENTRY_QUALIFIER_ASSOC_r1821546_c4",
        "SG_BIOENTRY_REF_ASSOC_r358337_c5",
        "SG_BIOENTRY_r184292_c9",
        "SG_BIOSEQUENCE_r184292_c6",
        "SG_DBXREF_r618244_c4",
        "SG_LOCATION_r1020335_c8",
        "SG_REFERENCE_r128816_c6",
        "SG_SEQFEATURE_QUALIFIER_ASSOC_r825116_c4",
        "SG_SEQFEATURE_r1020335_c6",
        "SG_TAXON_NAME_r105826_c3",
        "SPStock_r122497_c7",
        "Tax_r1000001_c15",
        "WDC_age_r10_c3",
        "WDC_astrology_r13_c7",
        "WDC_astronomical_r12_c6",
        "WDC_game_r9_c5",
        "WDC_kepler_r10_c4",
        "WDC_planetz_r11_c5",
        "WDC_satellites_r174_c8",
        "WDC_science_r10_c3",
        "WDC_symbols_r64_c3",
        # "ZBC00DT_COCM_r3175806_c35", # not available
        "amalgam1_denormalized_r51_c87",
        "census_r196295_c42",
        "chess_r28056_c7",
        "ditag_feature_r3960124_c13",
        "echocardiogram_r132_c13",
        "entytysrcgen_r26139_c46",
        "fd-reduced-30_r250001_c30",
        "flight_r1001_c109",
        "hepatitis_r155_c20",
        "horse_r300_c29",
        "iris_r147_c5",
        ["isolet_r7798_c618", {"cols": 200}],
        "letter_r18668_c17",
        "musicbrainz_denormalized_r79569_c100",
        ["ncvoter_allc_r7503554_c94", {"rows": 100000}],
        "ncvoter_r8060060_c19",
        "nursery_r12960_c9",
        "plista_r996_c63",
        "struct_sheet_range_r664128_c32",
        "t_biocase_gathering_agent_r72738_c18",
        "t_biocase_gathering_r90992_c35",
        "t_biocase_gathering_namedareas_r137711_c11",
        "t_biocase_gathering_sitecoordinates_r91349_c25",
        "t_biocase_identification_r91800_c38",
        "t_biocase_identification_highertaxon_r562959_c3",
        "t_biocase_measurementsorfacts_r3113_c24",
        "t_biocase_metadata_r4_c56",
        "t_biocase_multimediaobject_r18785_c15",
        "t_biocase_preparation_r81780_c21",
        "t_biocase_specimenunit_mark_r8976_c12",
        "t_biocase_unit_r91349_c14",
        ["uniprot_r539166_c223", {"rows": 1001, "cols": 120}],
        # "VTTS_r13042057_c75", # not available
        "lineitem_r6001215_c16",
        # timeout (1h) -> only 1 iteration
        ["ncvoter_allc_r7503554_c94", {"iterations": 1}],
        ["tpch_denormalized_r6001216_c52", {"iterations": 1}],
        # memory issues -> only 1 iteration
        ["uniprot_r539166_c223", {"iterations": 1}],
        ["isolet_r7798_c618", {"iterations": 1}],
        ["uniprot_r539166_c223", {"rows": 1001, "iterations": 1}],
        # timeout (8h) -> only 1 iteration
        ["ncvoter_allc_r7503554_c94", {"rows": 1500000, "timeout": 28800,
                                       "iterations": 1}]
    ],
    "options": {
        "iterations": 5
    }
}

#: Scaling experiments for HyUCC.
exp_hyucc_scaling = {
    "instances": [
        # isolet col scaling with only 3 iterations
        ["isolet_r7798_c618",
         {"cols": [40, 80, 120, 160, 200, 240, 280],
          "iterations": 3}],
        # col scaling for uniprot, and ncvoter_allc (with all and 100k rows)
        ["uniprot_r539166_c223",
         {"cols": [10, 15, 20, 25, 30, 35, 40, 45, 50]}],
        ["ncvoter_allc_r7503554_c94",
         {"rows": 100000,
          "cols": [10, 20, 30, 40, 50, 60, 70, 80, 90]}],
        ["ncvoter_allc_r7503554_c94",
         {"cols": [10, 15, 20, 25, 30, 35, 40]}],
        # row scaling for lineitem, VTTS, and ncvoter_allc
        # (ncvoter_allc with 8h timeout and 1 iteration)
        # ["VTTS_r13042057_c75", # not available
        #  {"rows": [1000000, 2000000, 3000000, 4000000, 5000000, 6000000,
        #            7000000, 8000000, 9000000, 10000000]}],
        ["lineitem_r6001215_c16",
         {"rows": [500000, 1000000, 1500000, 2000000, 2500000, 3000000,
                   3500000, 4000000, 4500000]}],
        ["ncvoter_allc_r7503554_c94",
         {"rows": [250000, 500000, 750000, 1000000, 1250000],
          "timeout": 28800,
          "iterations": 1}]
    ],
    "options": {
        "iterations": 5
    }
}

###############################################################################
# actually run the experiments

# global options
# eh.dry_run = True
eh.out_dir = "results/"

# create subtables
print("creating subtables")
run_experiment(eh.subtable_runs(
    flat_runs([exp_hpiv_run_time, exp_hpiv_optimizations, exp_hpiv_scaling,
               exp_hpiv_sampling, exp_hyucc_run_time, exp_hyucc_scaling])),
               eh.command_fun_hpiv, eh.result_fun_subtable, dry_run=eh.dry_run)

# run the HPIValid experiments
print("HPIValid run time experiments (ca. 6h 15min)")
eh.run_experiment_hpiv(exp_hpiv_run_time, "hpiv_run_time.csv")

print("HPIValid optimizations experiments (ca. 6h)")
eh.run_experiment_hpiv(exp_hpiv_optimizations, "hpiv_optimizations.csv")

print("HPIValid scaling experiments (ca. 13h 45min)")
eh.run_experiment_hpiv(exp_hpiv_scaling, "hpiv_scaling.csv")

print("HPIValid sampling experiments (ca. 1h 30min)")
eh.run_experiment_hpiv(exp_hpiv_sampling, "hpiv_sampling.csv")

# run the HyUCC experiments
print("HyUCC run time experiments (ca. 14h)")
eh.run_experiment_hyucc(exp_hyucc_run_time, "hyucc_run_time.csv")

print("HyUCC scaling experiments (> 17h)")
eh.run_experiment_hyucc(exp_hyucc_scaling, "hyucc_scaling.csv")
