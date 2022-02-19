# This Python file uses the following encoding: utf-8

# if__name__ == "__main__":
#     pass

import codecs,
import markdown
import pdfkit

def markdownToHtml(text)
    html = markdown.markdown(text)
    return html

def htmlToPDF(html,filePathName)
    pdfkit.from_string(html, filePathName)

