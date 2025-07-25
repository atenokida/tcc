################################################################################
## Plots for evaluating the low-level optimizations of HPIValid
##
## There are two low-level optimizations.  First, to make the
## validation more cache efficient, HPIValid copies the PLIs such that
## each PLI is represented by a consecutive block of memory.  Second,
## whenever the tree search has multiple edges of minimum cardinality
## to branch on, HPIValid uses a heuristic as tie-breaker that aims at
## speeding up the validation by making the clusters in the resulting
## PLIs small.

source("helper.R")
library(egg)
library(tikzDevice)

################################################################################
## loading and formatting the table

## read table
tbl <- read.csv(hpiv_optimizations)

## four configurations for whether or not to use the tie-breaking
## heuristic (H) and the PLI copying (C)
tbl$config <- ""
tbl[tbl$copy_PLIs == 1 & tbl$tiebreaker_heuristic == 1, ]$config <- "CH"
tbl[tbl$copy_PLIs == 0 & tbl$tiebreaker_heuristic == 1, ]$config <- "H"
tbl[tbl$copy_PLIs == 1 & tbl$tiebreaker_heuristic == 0, ]$config <- "C"
tbl[tbl$copy_PLIs == 0 & tbl$tiebreaker_heuristic == 0, ]$config <- "$\\emptyset$"

tbl$config <- factor(tbl$config, levels = c("$\\emptyset$", "H", "C", "CH"))

## preparations for stacked run time plots
tbl <- add_run_times(tbl)
data <- prep_for_stacked_plot(tbl, c("dataset", "rows", "cols", "config"))

################################################################################
## creating the plots

## selecting the datasets to show (and in which order to show them)
datasets <- c("ncvoter_r8060060_c19",
              "tpch_denormalized_r6001216_c52",
              "lineitem_r6001215_c16")

## build one plot for each dataset in datasets
plots <- list()
for (dataset in datasets) {
    ## sub-table/data for current dataset
    tbl.i <- tbl[tbl$dataset == dataset,]
    data.i <- data[data$dataset == dataset,]

    ## format the dataset name (LaTeX format + shortening)
    name <- trim_dataset_name(dataset)
    name <- LaTeX_format_dataset(name)
    if (dataset == "tpch_denormalized_r6001216_c52") {
        name <- "tpch_denorm"
        name <- LaTeX_format_dataset(name)
        name <- paste0(name, " ($\\SI{500}{k}$)")
    }
    name <- paste0("\\small{", name, "}")

    ## label for y-coordinate only for first plot
    if (length(plots) == 0) {
        y_label <- ylab("runtime (s)")
    } else {
        y_label <- theme(axis.title.y = element_blank())
    }

    ## create the plot and add it to the list
    pi <- ggplot(data.i, aes(fill = variable, y = value, x = config)) +
        geom_bar(position = "stack", stat = "summary", fun = "mean") +
        scale_fill_manual(values = colors.runtime) +
        geom_boxplot(data = tbl.i, aes(x = config, y = t_total, fill = NULL),
                     outlier.alpha = 1.0, outlier.size = 0.8, outlier.shape = 4,
                     lwd = 0.2, alpha = 0.5) +
        apply_theme + theme(legend.position = "none") +
        xlab(name) + 
        y_label
    plots <- append(plots, list(pi))
}

## arrange the plots in one row
p <- ggarrange(plots = plots, nrow = 1, ncol = length(plots))

################################################################################
## output

## output to pdf
ggsave(paste0("output/pdf/impl_detail.pdf"), plot = p,
       width = vldb_line_width, height = 1.6)

## load LaTeX package siunitx
if (!exists("tikz.load.siunitx")) {
    options(tikzLatexPackages = c(getOption("tikzLatexPackages"),
                                  "\\usepackage{siunitx}"))
    tikz.load.siunitx <- TRUE
}

## output to LaTeX
tikz(file = paste0("output/latex/plots/impl_detail.tex"), 
     width=vldb_line_width, height=1.6, base)
print(p)
dev.off()
