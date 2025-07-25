################################################################################
## Create the table
## 
## This creates the table with all results including the comparison
## between HyUCC and HPIValid.  As the table includes a column with
## the breakdown plots, these have to be also be created (with
## run_time_breakdown.R) for the resulting LaTeX table to compile.

source("helper.R")
library(xtable)

################################################################################
## loading and merging the tables for HPIValid and HyUCC

## read table for HPIValid, aggregate to median values grouped by
## instance and filter by run time
tbl.hpiv <- read.csv(hpiv_run_time)
tbl.hpiv <- aggregate(.~dataset+rows+cols, data=tbl.hpiv, FUN=median)
tbl.hpiv <- tbl.hpiv[tbl.hpiv$t_total > 0.001, ]

## read table for HyUCC, reformat to match the HPIValid table and
## aggregate to median values grouped by instance
tbl.hyucc <- read.csv(hyucc_run_time)
tbl.hyucc <- reformat_hyucc_data(tbl.hyucc)
tbl.hyucc <- aggregate(.~dataset+rows+cols, data=tbl.hyucc, FUN=median)

## merge the tables; all.x = TRUE makes sure that the resulting table
## has exactly one row for every row in the HPIValid table (even if
## the corresponding row is missing in the HyUCC table or if the HyUCC
## table has additional rows).
tbl <- merge(tbl.hpiv, tbl.hyucc, all.x = TRUE)

################################################################################
## cleaning the table and adding additional columns

## indicate missing values with -1 (so we see that something is missing
## in the table)
if (any(is.na(tbl$hyucc_memory))) {
    tbl[is.na(tbl$hyucc_memory), ]$hyucc_memory <- -1000
}
if (any(is.na(tbl$hyucc_t_total))) {
    tbl[is.na(tbl$hyucc_t_total), ]$hyucc_t_total <- -1
}

## clear entries, where HyUCC was out of memory (total time = 0)
tbl[tbl$hyucc_t_total == 0,]$hyucc_memory <- NA
tbl[tbl$hyucc_t_total == 0,]$hyucc_t_total_clean <- NA
tbl[tbl$hyucc_t_total == 0,]$hyucc_t_total <- NA

## column with the speedup
tbl$speedup <- tbl$hyucc_t_total_clean / tbl$t_total

## column for the breakdown plots
tbl$breakdown <- mapply(function (dataset, rows, cols) 
    paste0("\\hspace{-6pt}\\raisebox{-1.7pt}{\\resizebox{!}{8.8pt}{\\input{",
           "plots/breakdowns/", breakdown_file_name(dataset, rows, cols),
           "}}}\\hspace{-6pt}"), tbl$dataset, tbl$rows, tbl$cols)

## reducing to columns we still need
tbl <- tbl[,c("dataset", "rows", "cols", "ucc_count","t_total",
              "breakdown", "memory", "hyucc_t_total", "hyucc_t_total_clean",
              "hyucc_memory", "speedup")]

## sort by total time of HPIValid
tbl <- tbl[order(tbl$t_total),]

################################################################################
## formatting each row
tbl$dataset <- as.character(tbl$dataset)

for (i in 1:nrow(tbl)) {
    ## i <- 50
    row <- tbl[i, ]
    ## row$rows
    ## log10(row$rows)
    
    ## make numeric values numeric...
    row$rows <- as.numeric(row$rows)
    row$cols <- as.numeric(row$cols)
    row$ucc_count <- as.numeric(row$ucc_count)
    row$t_total <- as.numeric(row$t_total)
    row$memory <- as.numeric(row$memory)
    row$hyucc_t_total <- as.numeric(row$hyucc_t_total)
    row$hyucc_t_total_clean <- as.numeric(row$hyucc_t_total_clean)
    row$hyucc_memory <- as.numeric(row$hyucc_memory)
    row$speedup <- as.numeric(row$speedup)
    
    ## collecting footnotes
    footnotes <- c()
    if (row$t_total >= 3600) {
        ## timeout for HPIValid
        footnotes <- c(footnotes, "a)")
    }
    if (parse_rows(row$dataset) != row$rows |
        parse_cols(row$dataset) != row$cols) {
        ## subtable
        footnotes <- c(footnotes, "b)")
    }
    if (row$dataset == "ILOA_r20000000_c48" & row$rows == 20000000) {
        ## iloa cut down to 20M rows
        footnotes <- c(footnotes, "c)")
    }
    if (!is.na(row$hyucc_t_total) & row$hyucc_t_total == 28800) {
        ## run with 8h timeout
        footnotes <- c(footnotes, "d)")
    }
    
    ## formatting the dataset name (shorten + LaTeX-format + footnote)
    row$dataset <- trim_dataset_name(as.character(row$dataset))
    row$dataset <- gsub("gathering", "gath", row$dataset)
    row$dataset <- gsub("identification", "ident", row$dataset)
    row$dataset <- gsub("biocase", "bioc", row$dataset)
    ## row$dataset <- gsub("coordinates", "coord", row$dataset)
    ## row$dataset <- gsub("QUALIFIER", "QUAL", row$dataset)
    ## row$dataset <- gsub("measurementsorfacts", "measureorfacts", row$dataset)
    row$dataset <- LaTeX_format_dataset(row$dataset)
    row$dataset <- paste0("\\smaller{", row$dataset, "}")
    if (length(footnotes) != 0) {
        row$dataset <- paste0(row$dataset, "\\textsuperscript{",
                              paste(footnotes, collapse = ", "), "}")
    }

    ## format number of rows
    if (row$rows < 1e3) {
        row$rows <- format(row$rows, nsmall = 0)
    } else if (row$rows < 1e6) {
        nsmall <- 5 - floor(log10(row$rows))
        row$rows <- paste0(
            "\\SI{", format(row$rows/1e3, digits = 3, nsmall = nsmall), "}{k}")
    } else {
        nsmall <- 8 - floor(log10(row$rows))
        row$rows <- paste0(
            "\\SI{", format(row$rows/1e6, digits = 3, nsmall = nsmall), "}{M}")
    }

    ## format the number of columns
    row$cols <- format(row$cols, nsmall = 0)

    ## format the number of UCCs
    if (row$t_total < 3600) {
        row$ucc_count <- format(row$ucc_count, big.mark = ",")
    } else {
        row$ucc_count <- paste0(
            "$>$", format(floor(row$ucc_count/1e6), big.mark = ","), "\\,\\si{M}")
    }

    ## format the memory usage
    row$memory <- format(round(row$memory / 1000, digits = 0), big.mark = ",")
    if (!is.na(row$hyucc_memory)) {
        row$hyucc_memory <- format(
            round(row$hyucc_memory / 1000, digits = 0), big.mark = ",")
    }

    ## format speedup
    if (!is.na(row$speedup)) {
        if (row$hyucc_t_total < 3600) {
            row$speedup <- format(round(row$speedup, digits = 2), nsmall = 2)
        } else {
            row$speedup <- paste0(
                "$>$", format(floor(row$speedup), big.mark = ","))
        }
    }

    ## run time
    format_time <- function (time, timeout) {
        if (timeout) {
            paste0("$>$", format(round(time), big.mark = ","))
        } else if (time < 0.01) {
            paste0("\\SI{",
                   format(round(time * 1000, digits = 2), nsmall = 2),
                   "}{m}")
        } else {
            format(round(time, digits = 2), big.mark = ",", nsmall = 2)
        }
    }
    row$t_total <- format_time(row$t_total, row$t_total >= 3600)
    if (!is.na(row$hyucc_t_total)) {
        timeout <- row$hyucc_t_total >= 3600
        row$hyucc_t_total <- format_time(row$hyucc_t_total, timeout)
        row$hyucc_t_total <- paste0("{\\color{black!60!white}", row$hyucc_t_total, "}")
        row$hyucc_t_total_clean <- format_time(row$hyucc_t_total_clean, timeout)
    }
    
    tbl[i, ] <- row
}

################################################################################
## output to LaTeX

## bold formatting function for the
bold <- function(x) {
    paste('{\\textbf{',x,'}}', sep ='')
}

## LaTeX table header
header <- list()
header$pos <- list(0)
header$command <- c(
"\\rowcolor{white}  &  &  &  & \\multicolumn{3}{c|}{\\textbf{\\texttt{HPIValid}}} & \\multicolumn{4}{c}{\\textbf{\\texttt{HyUCC} Comparison}} \\\\
  {\\textbf{Dataset}} & {\\hspace{-0.75cm}\\textbf{Rows}} & {\\hspace{-0.25cm}\\textbf{Cols}} & {\\textbf{UCCs}} & {\\textbf{Time}} & {\\textbf{Time}} & {\\textbf{Mem}} & {\\textbf{\\color{black!60!white}Total}} & {\\textbf{Time}} & {\\textbf{Mem}} & {\\hspace{-0.2cm}\\textbf{Speedup}} \\hspace{-0.2cm} \\\\
 \\rowcolor{white} & {\\textbf{[\\#]}} & {\\textbf{[\\#]}} & {\\textbf{[\\#]}} & {\\textbf{[s]}} & {\\hspace{-0.2cm}\\textbf{Breakdown}\\hspace{-0.2cm}} & {\\textbf{[MB]}} & {\\textbf{\\color{black!60!white}[s]}} & {\\textbf{[s]}} & {\\textbf{[MB]}} & \\\\\n")

## print the table
xtbl <- xtable(tbl, align = "clrrr|rcr|rrrr")
print(xtbl,
      include.rownames = FALSE,
      include.colnames = FALSE,
      add.to.row = header,
      booktabs = TRUE,
      table.placement = "t",
      floating = FALSE,
      sanitize.text.function =
          function(str) str,
      sanitize.colnames.function=bold,
      file = "output/latex/plots/table.tex")

