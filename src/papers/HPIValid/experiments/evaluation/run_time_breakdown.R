################################################################################
## Create mini runtime breakdown plots
##
## This creates one separate runtime breakdown plot for each instance.
## The resulting plots include just a single stacked bar without any
## surrounding elements (no legend etc).  This is for inclusion in the
## table.

source("helper.R")
library(ggplot2)
library(tikzDevice)

## read the table and select required columns
tbl <- read.csv(hpiv_run_time)
tbl <- tbl[, c("dataset", "rows", "cols", "t_total", "t_total_prep",
               "t_read_table", "t_construct_clusters",
               "t_total_enum_algo", "t_sample_diff_sets",
               "t_intersect_clusters")]

## additional run times derived from the other times
tbl <- add_run_times(tbl)

## getting data for stacked run time plots
data <- prep_for_stacked_plot(tbl, c("dataset", "rows", "cols"))

## each instance is identified by the dataset, the number of rows and
## number of columns
instances <- unique(tbl[, c("dataset", "rows", "cols")])

## creating the mini-plots for the table, one for each instance
for (row in 1:nrow(instances)) {
    dataset <- as.character(instances[row, "dataset"])
    rows <- instances[row, "rows"]
    cols <- instances[row, "cols"]

    data.sub <- data[data$dataset == dataset &
                     data$rows == rows & data$cols == cols, ]
    
    p <- ggplot(data.sub, aes(fill = variable, y = value, x = dataset), border = 0) + 
        geom_bar(position = "fill", stat = "summary", fun = "mean") +
        coord_flip() +
        scale_fill_manual(values = colors.runtime) +
        theme_void() +
        theme(legend.position = "none")
    
    tikz(file = paste0("output/latex/plots/breakdowns/",
                       breakdown_file_name(dataset, rows, cols)),
         width = 5.5, height = 1)
    print(p)
    dev.off()
}


