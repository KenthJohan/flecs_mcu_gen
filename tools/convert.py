
# sudo apt-get update
# sudo apt-get install tesseract-ocr
# conda activate my-env
# pip install pytesseract
# pip install img2table
# pip install tesseract
# python convert.py

from img2table.document import PDF
from img2table.ocr import TesseractOCR

# Instantiation of the pdf
pdf = PDF(src="stm32g030c6.pdf")

# Instantiation of the OCR, Tesseract, which requires prior installation
ocr = TesseractOCR(lang="eng")

# Table identification and extraction
pdf_tables = pdf.extract_tables(ocr=ocr)

# We can also create an excel file with the tables
pdf.to_xlsx('tables.xlsx', ocr=ocr)