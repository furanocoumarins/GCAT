Usage: ./main_v2 file1.vcf file.sff3 [region] [REGION] [-t TYPES]
region format: CHR:START-END 
types format: comma-separated list, e.g. "gene,exon,enhancer". By default "all"

now works only for CM000665.2:START-END (chr 3)
If region is omitted, all variants are processed.