################################################################################
## Row and column scaling plots
##
## Creates scaling plots, where the scaling plot of each instances
## consists of three subplots: (i) The scaling behavior of HPIValid
## including run time breakdown, (ii) the development of the solution
## size (number of UCCs), and (iii) the comparison with HyUCC.

source("helper.R")
library(ggplot2)
library(egg)
library(tikzDevice)

#' Create the scaling plot for one instance.
#'
#' @param tbl.hpiv The table containing the HPIValid measurements.
#' @param tbl.hyucc The table containing the HyUCC measurements.  It
#'     should already be preprocessed to include the following
#'     columns: dataset, rows, cols, t_total, ucc_count
#' @param scaling_type "row" for row scaling and "column" for column
#'     scaling.
#' @param dataset The dataset.
#' @param fixed_dim_value The value of the non-varying dimension,
#'     i.e., for row scaling the number of columns and for column
#'     scaling the number of rows of the dataset.
#' @param max_x_hyucc The maximum x-coordinate for the comparison plot
#'     with hyucc.
#' @param leftmost Boolean value indicating whether this will be the
#'     leftmost plot.  If so, the label for the y-axis is printed and
#'     a legend is added to the comparison plot.
#' @param label_suffix A suffix to be added to the label of the
#'     dataset (e.g., to add the info that the dataset was reduced to
#'     a particular number of rows/cols).
#' @return A list of three plot objects (to be arranged from top to
#'     bottom).  The first shows the run time break down of HPIValid.
#'     The second shows the number of UCC using the same x-axis.  The
#'     third shows the comparison with HyUCC.
#' @examples
#' create_plot(tbl.hpiv, tbl.hyucc, "column", "ncvoter_allc_r7503554_c94", 100000, 90, TRUE, label_suffix = " (100k rows)")
#' create_plot(tbl.hpiv, tbl.hyucc, "row", "VTTS_r13042057_c75", 75, 13000000, TRUE)
create_plot <- function (tbl.hpiv, tbl.hyucc, scaling_type,
                         dataset, fixed_dim_value, max_x_hyucc,
                         leftmost, label_suffix = "") {
    ##----------------------------------------------------------------
    ## preparing the data

    ## rename the rows and cols columns: the one that is the variable
    ## on the x-axis (rows for row scaling, cols for column scaling)
    ## gets the name x_dim, the other gets the name fixed_dim.
    if (scaling_type == "row") {
        names(tbl.hpiv)[names(tbl.hpiv) == "rows"] <- "x_dim"
        names(tbl.hpiv)[names(tbl.hpiv) == "cols"] <- "fixed_dim"
        names(tbl.hyucc)[names(tbl.hyucc) == "rows"] <- "x_dim"
        names(tbl.hyucc)[names(tbl.hyucc) == "cols"] <- "fixed_dim"
    } else { ## column
        names(tbl.hpiv)[names(tbl.hpiv) == "rows"] <- "fixed_dim"
        names(tbl.hpiv)[names(tbl.hpiv) == "cols"] <- "x_dim"
        names(tbl.hyucc)[names(tbl.hyucc) == "rows"] <- "fixed_dim"
        names(tbl.hyucc)[names(tbl.hyucc) == "cols"] <- "x_dim"
    }

    ## select the dataset we are interested in
    tbl.hpiv <- tbl.hpiv[tbl.hpiv$dataset == dataset &
                         tbl.hpiv$fixed_dim == fixed_dim_value, ]
    tbl.hyucc <- tbl.hyucc[tbl.hyucc$dataset == dataset &
                           tbl.hyucc$fixed_dim == fixed_dim_value, ]

    ## return if no data exists for the dataset
    if (nrow(tbl.hpiv) == 0) {
        return(list(ggplot(), ggplot(), ggplot()))
    }

    ## additional run times derived from the other times
    tbl.hpiv <- add_run_times(tbl.hpiv)

    ## getting data for stacked run time plots
    data <- prep_for_stacked_plot(tbl.hpiv, c("dataset", "x_dim"))

    ## remove data points where HyUCC had a timeout
    tbl.hyucc <- tbl.hyucc[tbl.hyucc$ucc_count != 0, ]

    ## create a combined table for HPIValid and HyUCC
    tbl.hpiv$algo <- algo.hpiv
    tbl.hyucc$algo <- algo.hyucc
    tbl <- rbind(
        tbl.hpiv[, c("dataset", "x_dim", "fixed_dim", "t_total", "algo")],
        tbl.hyucc[, c("dataset", "x_dim", "fixed_dim", "t_total", "algo")])

    ##----------------------------------------------------------------
    ## create the plots

    ## factors for the box plot
    tbl.hpiv$x_dim <- as.factor(tbl.hpiv$x_dim)
    data$x_dim <- as.factor(data$x_dim)

    ## formatting the x-axis
    tick.breaks = levels(data$x_dim)[c(FALSE, TRUE)]
    tick.labels <- sapply(as.numeric(tick.breaks), FUN = number_with_unit)
    x_axis <- scale_x_discrete(labels = tick.labels, breaks = tick.breaks)

    ## formatting the y-axis
    if (leftmost) {
        hide_ylab <- theme()
    } else {
        hide_ylab <- theme(axis.title.y = element_blank())
    }

    # first plot: run time breakdown of HPIValid
    p1 <- ggplot() +
        geom_area(data = data, aes(y = value, x = x_dim, fill = variable, group = variable),
                  stat = "summary", fun = "mean") +
        geom_boxplot(data = tbl.hpiv, aes(x = x_dim, y = t_total), position = position_dodge(),
                     width = 0.3, lwd = 0.2, alpha = 0.5, 
                     outlier.alpha = 1.0, outlier.size = 0.4, outlier.shape = 4) +
        apply_theme + hide_x_axis + x_axis +
        ylab("time [s]") + 
        hide_ylab +
        theme (legend.position = "none")

    ## second plot: number of UCCs
    p2 <- ggplot(tbl.hpiv, aes(x = x_dim, y = ucc_count, group = dataset)) +
        stat_summary(fun = median, geom = "point") + 
        stat_summary(fun = median, geom = "line", aes(group = dataset)) +
        apply_theme + x_axis + 
        scale_y_continuous(labels = function(x) number_with_unit(x, 1), limits = lim) + 
        ylab("UCCs") + 
        hide_ylab +
        xlab("")

    ## formatting the legend
    if (leftmost) {
        legend <- theme(legend.position = c(0.02, 0.98),
                        legend.justification = c("left", "top"),
                        legend.background = element_blank(),
                        legend.box.background = element_blank(),
                        legend.margin = margin(t = -0.1, r = 0.1,
                                               b = 0.1, l = 0.1, unit = "cm"),
                        legend.key.size = unit(0.5, "cm"),
                        legend.spacing.x = unit(0.1, "cm"))
    } else {
        legend <- theme (legend.position = "none")
    }

    ## third plot: comparison with HyUCC
    p3 <- ggplot() +
        stat_summary(geom = "line", data = tbl, 
                     aes(x = x_dim, y = t_total, col = algo), fun = median) +
        stat_summary(geom = "point", data = tbl, 
                     aes(x = x_dim, y = t_total, col = algo), fun = median) +
        scale_x_continuous(
            limits = c(min(tbl$x_dim), max_x_hyucc),
            labels = tick.labels,
            minor_breaks = NULL,
            breaks = as.numeric(tick.breaks)) +
        hide_legend_name +
        apply_colors + apply_theme + normal_y_axis + 
        scale_y_continuous(labels = function(x) number_with_unit(x, 1)) +
        ylab("time [s]") + 
        hide_ylab +
        xlab(paste0("number of ", scaling_type, "s\n",
                    LaTeX_format_dataset(trim_dataset_name(dataset)),
                    label_suffix)) +
        legend

    ## combine the plots
    list(p1 + theme(plot.margin = margin(5.5, 5.5, -2, 5.5)),
         p2 + theme(plot.margin = margin(5.5, 5.5, -5.5, 5.5)),
         p3)
}

################################################################################
## load and preprocess the data
tbl.hpiv <- read.csv(hpiv_scaling)
tbl.hyucc <- read.csv(hyucc_scaling)
tbl.hyucc <- reformat_hyucc_data(tbl.hyucc)
names(tbl.hyucc)[names(tbl.hyucc) == 'hyucc_t_total_clean'] <- 't_total'
names(tbl.hyucc)[names(tbl.hyucc) == 'hyucc_ucc_count'] <- 'ucc_count'

################################################################################
## column scaling
p1 <- create_plot(tbl.hpiv, tbl.hyucc, "column",
                  "ncvoter_allc_r7503554_c94", 100000, 90, TRUE,
                  label_suffix = " (100k rows)")
p2 <- create_plot(tbl.hpiv, tbl.hyucc, "column",
                  "ncvoter_allc_r7503554_c94", 7503554, 60, FALSE)
p3 <- create_plot(tbl.hpiv, tbl.hyucc, "column",
                  "uniprot_r539166_c223", 539166, 100, FALSE)

p <- ggarrange(p1[[1]], p2[[1]], p3[[1]],
               p1[[2]], p2[[2]], p3[[2]],
               p1[[3]], p2[[3]], p3[[3]],
               nrow=3, ncol=3, heights = c(2, 1, 2))

## save to pdf and tikz
ggsave(paste0("output/pdf/scaling_col_breakdown.pdf"), plot = p, 
       width = vldb_text_width, height = 4)

if (!exists("tikz.load.siunitx")) {
    options(tikzLatexPackages = c(getOption("tikzLatexPackages"),
                                  "\\usepackage{siunitx}"))
    tikz.load.siunitx <- TRUE
}

tikz(file = paste0("output/latex/plots/scaling_col_breakdown.tex"), 
     width=vldb_text_width, height=4, base)
print(p)
dev.off()

################################################################################
## row scaling
p1 <- create_plot(tbl.hpiv, tbl.hyucc, "row",
                  "VTTS_r13042057_c75", 75, 13000000, TRUE)
p2 <- create_plot(tbl.hpiv, tbl.hyucc, "row",
                  "lineitem_r6001215_c16", 16, 6000000, FALSE)
p3 <- create_plot(tbl.hpiv, tbl.hyucc, "row",
                  "ncvoter_allc_r7503554_c94", 94, 2500000, FALSE)

p <- ggarrange(p1[[1]], p2[[1]], p3[[1]],
               p1[[2]], p2[[2]], p3[[2]],
               p1[[3]], p2[[3]], p3[[3]],
               nrow=3, ncol=3, heights = c(2, 1, 2))

## save to pdf and tikz
ggsave(paste0("output/pdf/scaling_row_breakdown.pdf"), plot = p, 
       width = vldb_text_width, height = 4)

tikz(file = paste0("output/latex/plots/scaling_row_breakdown.tex"), 
     width=vldb_text_width, height=4, base)
print(p)
dev.off()
