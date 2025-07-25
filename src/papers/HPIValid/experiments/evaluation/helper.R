################################################################################
## Some helper functions and basic settings

library(reshape2)
library(ggplot2)

################################################################################
## File names

## hpiv files
hpiv_run_time      <- "../results_final/hpiv_run_time.csv" 
hpiv_scaling       <- "../results_final/hpiv_scaling.csv"
hpiv_sampling      <- "../results_final/hpiv_sampling.csv"
hpiv_optimizations <- "../results_final/hpiv_optimizations.csv"

## hyucc files
hyucc_run_time     <- "../results_final/hyucc_run_time.csv"
hyucc_scaling      <- "../results_final/hyucc_scaling.csv"


################################################################################
## Data preparation

#' Derive additional run times from HPIValid measurements.
#'
#' @param tbl The table containing the HPIValid measurements.
#' @return A new table additional columns t_free_memory and
#'     t_sample_diff_sets.
add_run_times <- function(tbl) {
    tbl$t_free_memory <- tbl$t_total_prep -
        tbl$t_read_table - tbl$t_construct_clusters
    tbl$t_tree_search <- tbl$t_total_enum_algo -
        tbl$t_sample_diff_sets - tbl$t_intersect_clusters
    tbl
}

#' Preprocessing HyUCC measurements.
#'
#' @param tbl.hyucc The table containing the HyUCC measurements.
#' @return Cleaned table where in particular all run times are in
#'     seconds.
reformat_hyucc_data <- function(tbl.hyucc) {
    tbl.hyucc$hyucc_t_total <- tbl.hyucc$hyucc_t_total_ms / 1000
    tbl.hyucc$hyucc_t_safepoint <- tbl.hyucc$hyucc_t_safepoint_ns / 1e9
    tbl.hyucc$hyucc_t_total_clean <- tbl.hyucc$hyucc_t_total - tbl.hyucc$hyucc_t_safepoint
    tbl.hyucc[, c("dataset", "rows", "cols", "hyucc_t_total", "hyucc_t_total_clean",
                  "hyucc_t_safepoint", "hyucc_memory", "hyucc_ucc_count")]
}

#' Prepare HPIValid measurement for breakdown plots.
#'
#' @param tbl The table containing HPIValid measurements.  The table
#'     should contain the columns t_free_memory and t_tree_search (see
#'     \code{\link{add_run_times}}).
#' @param identifier_cols The columns that together identify values
#'     describing the same measurements.
#' @return The data necessary for stacked plots.
prep_for_stacked_plot <- function(tbl, identifier_cols) {
    tbl.data <- tbl[, c(identifier_cols, c("t_tree_search",
                        "t_intersect_clusters", "t_sample_diff_sets",
                        "t_free_memory", "t_construct_clusters",
                        "t_read_table"))]
    names(tbl.data)[names(tbl.data) == "t_tree_search"] <- "Tree Search"
    names(tbl.data)[names(tbl.data) == "t_intersect_clusters"] <- "Validation"
    names(tbl.data)[names(tbl.data) == "t_sample_diff_sets"] <- "Sample Difference Sets"
    names(tbl.data)[names(tbl.data) == "t_free_memory"] <- "Free Memory"
    names(tbl.data)[names(tbl.data) == "t_construct_clusters"] <- "Construct Clusters"
    names(tbl.data)[names(tbl.data) == "t_read_table"] <- "Read Table"

    melt(tbl.data, id.vars = identifier_cols)
}

################################################################################
## Formatting

## LaTeX formatted algorithm names
algo.hpiv <- "\\texttt{HPIValid}"
algo.hyucc <- "\\texttt{HyUCC}"

#'Remove trailing size indicators from dataset name
#'
#' @param dataset Name of the dataset.
#' @return Name of the dataset without the "_r??_c??" part.
trim_dataset_name <- function(dataset) {
    split <- strsplit(dataset, "_", fixed = TRUE)[[1]]
    split <- split[1:(length(split) - 2)]
    paste(split, collapse = "_")
}

#' Format dataset name for use in LaTeX
#'
#' @param dataset Name of the dataset.
#' @return Dataset name with escaped underscores and set with \texttt.
LaTeX_format_dataset <- function(dataset) {
    x <- as.character(dataset)
    x <- gsub("_", "\\\\_", x)
    paste0("\\texttt{", x, "}")
}

#' Formatting large numbers using units k and M.
#'
#' @param num A number or a vector of numbers.
#' @param digits The number of digits to be used.
#' @return A number (or vector of numbers) represented using k (for
#'     numbers >= 1k) and M (for numbers >= 1M), rounded to the given
#'     number of digits.  If a given element is not a number it is not
#'     changed.
number_with_unit <- function(num, digits = 0) {
    if (!is.numeric(num)) {
        num
    } else if (is.vector(num) & length(num) > 1) {
        sapply(num, function (x) number_with_unit(x, digits))
    } else if (is.na(num)) {
        num
    } else if (num < 1000) {
        round(num, digits = digits)
    } else if (num < 1000000) {
        paste0("\\SI{", round(num / 1000, digits = digits), "}{k}")
    } else {
        paste0("\\SI{", round(num / 1000000, digits = digits), "}{M}")
    }
}

#' Filename for the mini breakdown plots.
#'
#' @param dataset Name of the dataset.
#' @param rows Number of rows of the dataset.
#' @param cols Number of columns of the dataset.
#' @return Filename (without folder path) to the breakdown plot.
breakdown_file_name <- function(dataset, rows, cols) {
    paste0(paste(dataset, rows, cols, sep = "_"), ".tex")
}


################################################################################
## Plot style

## latex widths in vldb style in inch accounting for scaling to fit
## small font size
vldb_line_width <- 8.4 / 2.54 * 9 / 8
vldb_text_width <- 17.7 / 2.54 * 9 / 8

## color blind friendly colors for the run time breakdown
colors.runtime <- c("#D55E00", "#F0E442", "#E69F00", "#999999",
                    "#009E73", "#56B4E9")

## additional colors for isolet scaling
colors <- c("#A63335", "#0072B2", "#858200")

## use the colors
apply_colors <- list(
    scale_fill_manual(values = colors.runtime),
    scale_color_manual(values = colors))

## general theme
apply_theme <- list(
    theme_bw(), 
    apply_colors, 
    theme(plot.margin = margin(5.5, 5.5, 0, 5.5, "pt"))
)

#' Adds a small offset to the limits in a plot.
#'
#' @param auto The limits automatically generated by ggplot.
#' @return Slightly increased limits.
lim <- function (auto) {
    offset <- (auto[2] - auto[1]) * 0.05
    if (offset == 0) {
        offset = 1
    }
    c(auto[1] - offset, auto[2] + offset)
}

#' Adds a small offset to the limits in a plot with log axis.
#'
#' @param auto The limits automatically generated by ggplot.
#' @return Slightly increased limits.
lim_log <- function (auto) {
    offset <- (auto[2] / auto[1])^0.05
    c(auto[1] / offset, auto[2] * offset)
}

## default formatting of a logarithmic y-axis
log_y_axis <- list(
    scale_y_log10(
        breaks = 10^(-10:10), 
        minor_breaks = rep(1:9, 21)*(10^rep(-10:10, each=9)),
        labels = number_with_unit(10^(-10:10), digits = 10),
        limits = lim_log))

## default formatting of a logarithmic y-axis, skipping every other
## break
log_y_axis_even <- list(
    scale_y_log10(
        breaks = 10^(2 * (-10:10)), 
        minor_breaks = rep(1:9, 21)*(10^rep(-10:10, each=9)),
        labels = paste0("$10^{", 2 * (-10:10), "}$"),
        limits = lim_log))

## default formatting of a (non-logarithmic) y-axis
normal_y_axis <- scale_y_continuous(limits = lim)

## hiding the x-axis
hide_x_axis <- theme(axis.title.x = element_blank(), 
                     axis.text.x = element_blank(),
                     axis.ticks.x = element_blank())

## hiding the legend name
hide_legend_name <- guides(fill = guide_legend(title=NULL), 
                           color = guide_legend(title=NULL))

################################################################################
## Functions for parsing filenames of tables and subtables

#' The name of a dataset given the filename.
#'
#' This is particularly helpful when dealing with file names of
#' subtables.
#'
#' @param filename The filename of a table or a subtable.
#' @return The dataset name including the "_c??_r??" part.
parse_dataset <- function(filename) {
    split <- strsplit(filename, "/", fixed = TRUE)[[1]]
    if (length(split) == 3) {
        split[2]
    } else {
        strsplit(split[length(split)], ".", fixed = TRUE)[[1]][1]
    }
}

#' Deduce the number of rows from the filename.
#'
#' @param filename The filename of a table or a subtable.
#' @return The number of rows of the table or subtable.
parse_rows <- function(filename) {
    if (parse_is_subtable(filename)) {
        rows <- parse_rows_sub(filename)
        if (rows == 0) {
            parse_rows_full(filename)
        } else {
            rows
        }
    } else {
        parse_rows_full(filename)
    }
}

#' Deduce the number of columns from the filename.
#'
#' @param filename The filename of a table or a subtable.
#' @return The number of columns of the table or subtable.
parse_cols <- function(filename) {
    if (parse_is_subtable(filename)) {
        cols <- parse_cols_sub(filename)
        if (cols == 0) {
            parse_cols_full(filename)
        } else {
            cols
        }
    } else {
        parse_cols_full(filename)
    }
}

#' Check whether a filename represents a subtable.
#'
#' @param filename The filename of a table or a subtable.
#' @return True if and only if the given filename represents a
#'     subtable.
parse_is_subtable <- function(filename) {
    split <- strsplit(filename, "/", fixed = TRUE)[[1]]
    return(length(split) == 3)
}

#' Deduce the number of rows from the subtable filename.
#'
#' @param filename The filename of a subtable.
#' @return The number of rows of the subtable.
parse_rows_sub <- function(filename) {
    split <- strsplit(filename, "/", fixed = TRUE)[[1]]
    split <- split[length(split)]
    split <- strsplit(split, ".", fixed = TRUE)[[1]][1]
    split <- strsplit(split, "_", fixed = TRUE)[[1]]
    as.numeric(substring(split[length(split) - 1], 2))
}

#' Deduce the number of rows from the table filename.
#'
#' @param filename The filename of a table (not a subtable)
#' @return The number of rows of the table.
parse_rows_full <- function(filename) {
    dataset <- parse_dataset(filename)
    split <- strsplit(dataset, "_", fixed = TRUE)[[1]]
    as.numeric(substring(split[length(split) - 1], 2))
}

#' Deduce the number of columns from the subtable filename.
#'
#' @param filename The filename of a subtable.
#' @return The number of columns of the subtable.
parse_cols_sub <- function(filename) {
    split <- strsplit(filename, "/", fixed = TRUE)[[1]]
    split <- split[length(split)]
    split <- strsplit(split, ".", fixed = TRUE)[[1]][1]
    split <- strsplit(split, "_", fixed = TRUE)[[1]]
    as.numeric(substring(split[length(split)], 2))
}

#' Deduce the number of columns from the table filename.
#'
#' @param filename The filename of a table (not a subtable)
#' @return The number of columns of the table.
parse_cols_full <- function(filename) {
    dataset <- parse_dataset(filename)
    split <- strsplit(dataset, "_", fixed = TRUE)[[1]]
    as.numeric(substring(split[length(split)], 2))
}
