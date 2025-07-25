################################################################################
## Plots for the column scaling of the isolet instance
##
## Comparison plot between HPIValid and HyUCC for run time, memory
## consumption, number of UCCs, and time per UCC.

source("helper.R")
library(ggplot2)
library(egg)
require(scales)
library(tikzDevice)

################################################################################
## preparing the datasets

## read the HPIValid table and select relevant columns
tbl.hpiv <- read.csv(hpiv_scaling)
tbl.hpiv <- tbl.hpiv[, c("dataset", "rows", "cols", "t_total",
                         "memory", "ucc_count")]

## read the HyUCC table and rename columns to match names of HPIValid
tbl.hyucc <- read.csv(hyucc_scaling)
tbl.hyucc <- reformat_hyucc_data(tbl.hyucc)
names(tbl.hyucc)[names(tbl.hyucc) == 'hyucc_t_total_clean'] <- 't_total'
names(tbl.hyucc)[names(tbl.hyucc) == 'hyucc_memory'] <- 'memory'
names(tbl.hyucc)[names(tbl.hyucc) == 'hyucc_ucc_count'] <- 'ucc_count'
tbl.hyucc <- tbl.hyucc[, c("dataset", "rows", "cols", "t_total",
                           "memory", "ucc_count")]

## remove HyUCC instances that had a timeout (> 3600s)
tbl.hyucc <- tbl.hyucc[tbl.hyucc$t_total <= 3600,]

## additional column indicating the algorithm and combine the tables
tbl.hpiv$algo <- algo.hpiv
tbl.hyucc$algo <- algo.hyucc
tbl <- rbind(tbl.hpiv, tbl.hyucc)

## select the isolet dataset
tbl <- tbl[tbl$dataset == "isolet_r7798_c618", ]

## mark runs of HPIValid that hit the 900s timeout
tbl$timeout <- (tbl$t_total >= 900 & tbl$algo == algo.hpiv)

################################################################################
## plotting

## x-axis shared by all plots
x_axis <- scale_x_continuous(breaks = seq(40, 400, by = 80),
                             limits = c(min(tbl$col), max(tbl$col)))

## we always draw lines with points (for different data in different
## plots)
points_and_lines <- list (
    stat_summary(fun = median, geom = "line"),
    stat_summary(fun = median, geom = "point", fill = "white"),
    scale_shape_manual(values = c(19, 21)))

## run time plot
p1 <- ggplot(
    tbl,
    aes(x = cols, y = t_total,
        col = algo, linetype = timeout, shape = timeout)) + 
    points_and_lines + log_y_axis + x_axis + apply_theme + hide_x_axis +
    ylab("run time [s]")

## memory consumption
p2 <- ggplot(
    tbl[!is.na(tbl$memory), ],
    aes(x = cols, y = memory / 1e6,
        col = algo, linetype = timeout, shape = timeout)) + 
    points_and_lines + log_y_axis + x_axis + apply_theme + hide_x_axis +
    ylab("memory\n[GB]")

## number of UCCs
p3 <- ggplot(
    tbl[tbl$algo == algo.hpiv,],
    aes(x = cols, y = ucc_count, linetype = timeout, shape = timeout)) + 
    points_and_lines + log_y_axis + x_axis + apply_theme + hide_x_axis + 
    ylab("UCCs")

## time per UCC
p4 <- ggplot(
    tbl,
    aes(x = cols, y = 1e3 * t_total / ucc_count,
        col = algo, linetype = timeout, shape = timeout)) + 
    points_and_lines + x_axis + apply_theme + normal_y_axis + 
    ylab("time per\nUCC [ms]") + xlab("number of columns")

## aggregate the plots into one column
p <- ggarrange(
    p1 + theme(legend.position = "none"),
    p2 + theme(legend.position = "none"),
    p3 + theme(legend.position = "none"),
    p4 + theme(legend.position = "bottom",
               legend.margin=margin(0, 5.5, 0, 5.5),
               legend.box.margin=margin(-10, 0, 0, -25),
               legend.text = element_text(margin = margin(r = -25))) +
    geom_point(aes(fill = "timeout", shape = NA)) +
    guides(color = guide_legend(title=NULL, order = 2),
           fill = guide_legend(title=NULL,
                               override.aes=list(shape = 21,
                                                 col = colors[1],
                                                 fill = "white")),
           linetype = FALSE,
           shape = FALSE),
    heights = c(1.5, 1, 1, 1),
    nrow = 4)

## save to pdf and tikz
ggsave("output/pdf/scaling_columns_isolet.pdf", plot = p, 
       width = vldb_line_width, height = 3.5)

if (!exists("tikz.load.siunitx")) {
    options(tikzLatexPackages = c(getOption( "tikzLatexPackages" ), "\\usepackage{siunitx}"))
    tikz.load.siunitx <- TRUE
}

tikz(file = "output/latex/plots/scaling_columns_isolet.tex",
      width = vldb_line_width, height = 3.5)
print(p)
dev.off()

