# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

import subprocess, os

project = 'icubaby'
copyright = '2024, Paul Bowen-Huggett'
author = 'Paul Bowen-Huggett'
release = '1.1.0'

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = [ 'breathe' ]

# Breathe configuration
breathe_default_project = 'icubaby'
breathe_projects = {
    'icubaby': 'docs/'
}

templates_path = ['_templates']
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']

# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = 'alabaster'
html_static_path = ['_static']


read_the_docs_build = os.environ.get('READTHEDOCS', None) == 'True'
if read_the_docs_build:
     subprocess.call('doxygen', shell=True)

