################################################################################
## Plot run times depending on the sampling exponent
##
## Whenever we have p record pairs to choose from, sample only p^x
## pairs, where x is the sampling exponent.  To figure out a good
## choice for x, we look at run times depending on x.

source("helper.R")
library(ggplot2)
library(tikzDevice)

## load the data and select datasets
tbl <- read.csv(hpiv_sampling)
datasets <- c("census_r196295_c42",
              "hepatitis_r155_c20",
              "ncvoter_allc_r7503554_c94")
tbl <- tbl[tbl$dataset %in% datasets, ]

## the enumeration time sans the sampling of difference sets
tbl$t_enum <- tbl$t_total_enum_algo - tbl$t_sample_diff_sets

## reduce to relevant columns and format the column names
tbl <- tbl[, c("dataset", "rows", "cols", "sample_exponent",
               "t_total_enum_algo", "t_sample_diff_sets", "t_enum")]
colnames(tbl) <- c("dataset", "rows", "cols", "exponent",
                   "total time", "sampling", "enum.")

## melt table (i.e., have the different times not as different columns
## but only one time column (called value) and one column indicating
## the type of time (called variable))
tbl.melt <- melt(tbl, id.vars = c("dataset", "rows", "cols", "exponent"))

## LaTeX formatting for the datasets
tbl.melt$dataset <- sapply(tbl.melt$dataset, function (x)
    LaTeX_format_dataset(trim_dataset_name(as.character(x))))

## fix an order for the datasets: ncvoter_allc -> census -> hepatitis
tbl.melt$dataset <- factor(
    tbl.melt$dataset,
    levels = c("\\texttt{ncvoter\\_allc}",
               "\\texttt{census}",
               "\\texttt{hepatitis}"))

## make one guide legend that can be used for linetype and shape, as
## the time type varies both of them and we only want one time legend
## (also: put the time legend after the dataset legend)
g_time <- guide_legend("Time", order = 2)

## create the plot
p <- ggplot(tbl.melt, aes(x = exponent, y = value, col = dataset,
                          linetype = variable, shape = variable)) +
    stat_summary(geom = "line", fun = median) +
    stat_summary(geom = "point", size = 1.3, fun = median) +
    log_y_axis_even + apply_theme  +
    theme( ## legend positioning
        legend.position="right", legend.box = "vertical",
        legend.direction = "vertical", legend.title = element_blank(),
        legend.margin = margin(t = -0.1, r = 0.1, b = 0.1, l = -0.3, unit = "cm"),
        legend.spacing = unit(0, "cm")) +
    guides(color = guide_legend("Instance", order = 1),
           linetype = g_time, shape = g_time) + 
    xlab("sample exponent") + ylab("time [s]")

## save as pdf
ggsave("output/pdf/sampling.pdf", plot = p, width = vldb_line_width, height = 1.8)

## save as tikz
tikz(file = "output/latex/plots/sampling.tex", 
     width=vldb_line_width, height=1.7, base)
print(p)
dev.off()

