"""
Actually output the UCCs as well as the final hypergraph.

@author: Thomas Blaesius
"""

import experiments_hpiv as eh

#: Experiment description
exp = {
    "instances": [
        # just printing the header
        ["-", {"header": True}],
        # low run times (< 1min)
        "CE4HI01_r1678782_c65",
        "Hospital_r114920_c15",
        "ILOA_r20000000_c48",
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
        "ZBC00DT_COCM_r3175806_c35",
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
        "VTTS_r13042057_c75",
        "lineitem_r6001215_c16",
        "ncvoter_allc_r7503554_c94",
        ["tpch_denormalized_r6001216_c52", {"rows": [100000, 500000, 0]}]
    ],
    "options": {
        "iterations": 1
    }
}

# global options
# eh.dry_run = True
eh.out_dir = "results_with_output/"
eh.glob_opt_hpiv["no-output"] = False

# run the HPIValid experiments
eh.run_experiment_hpiv(exp, "hpiv_run_with_output.csv")
