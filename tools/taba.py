import pandas as pd
import tabula
file = "stm32g030c6.pdf"
df = tabula.read_pdf(file, pages = '30', multiple_tables = True)
print(df)