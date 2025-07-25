################################################################################
## Collect stats that explain the efficiency of HPIValid
##
## The **relative resample rate** is the number of difference sets
## computed after the initial sampling divided by the number of
## difference sets computed in the initial sampling.  It bounds how
## much sampling has to be done.
##
## The **solution overhead** is the number of non-root tree nodes per
## solution.  It is a measure for the efficiency of the tree search.

source("helper.R")
library(xtable)

################################################################################
## prepare data

## read the table and reduce it to the relevant columns
tbl <- read.csv(hpiv_run_time)
tbl <- tbl[, c("dataset", "rows", "cols", "tree_nodes", "ucc_count",
               "diff_sets", "diff_sets_initial")]

## take median for each instance (given by dataset, rows, and cols)
tbl <- aggregate(.~dataset+rows+cols, data=tbl, FUN=median)

## number of difference set sampled after the initial sampling divided
## by number of initially sampled difference sets (the "- 1" accounts
## for the fact that tbl$diff_sets includes the initial sampling)
tbl$relative_resample_rate <- tbl$diff_sets / tbl$diff_sets_initial - 1

## no sampling is done in instances without repeating values (all PLI
## clusters are trivial) -> 0 / 0 - 1 is nan -> count it as 0
tbl$relative_resample_rate[is.nan(tbl$relative_resample_rate)] <- 0

## number of tree nodes (excluding the root) per UCC
tbl$solution_overhead <- tbl$tree_nodes / tbl$ucc_count

################################################################################
## output

# datasets with relative resample rate > 1
xtbl.1 <- xtable(tbl[tbl$relative_resample_rate > 1,
                     c("dataset", "rows", "cols", "relative_resample_rate")],
                 align = "clrrr",
                 caption = "Instances with relative resample rate $> 1$.")
print(xtbl.1,
      include.rownames = FALSE,
      booktabs = TRUE,
      file = "output/latex/plots/efficiency_rel_sample_rate_large.tex")

## median, mean, and max values for different measurements
result <- data.frame(
    measurement = c("relative resample rate restricted to values $\\le 1$",
                    "solution overhead if nr. UCCs $= 1$",
                    "solution overhead if nr. UCCs $> 1$"),
    median = c(median(tbl$relative_resample_rate[tbl$relative_resample_rate <= 1]),
               median(tbl$solution_overhead[tbl$ucc_count == 1]),
               median(tbl$solution_overhead[tbl$ucc_count > 1])),
    mean = c(mean(tbl$relative_resample_rate[tbl$relative_resample_rate <= 1]),
             mean(tbl$solution_overhead[tbl$ucc_count == 1]),
             mean(tbl$solution_overhead[tbl$ucc_count > 1])),
    max = c(max(tbl$relative_resample_rate[tbl$relative_resample_rate <= 1]),
            max(tbl$solution_overhead[tbl$ucc_count == 1]),
            max(tbl$solution_overhead[tbl$ucc_count > 1])))

xtbl.2 <- xtable(result,
                 align = "clrrr",
                 caption = "Measurements for the relative resample rate and the solution overhead.",
                 digits = c(0, 0, 6, 6, 6))
print(xtbl.2,
      include.rownames = FALSE,
      booktabs = TRUE,
      floating.environment = "table*",
      sanitize.text.function = function(str) str,
      file = "output/latex/plots/efficiency.tex")
