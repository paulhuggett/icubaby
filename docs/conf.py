# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

import os
import subprocess

project = 'icubaby'
copyright = '2024, Paul Bowen-Huggett'
author = 'Paul Bowen-Huggett'
version = '1.1.0'
release = version

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = [ 'breathe' ]

# Breathe configuration
breathe_default_project = 'icubaby'
breathe_projects = {
    'icubaby': './xml/'
}

templates_path = ['_templates']
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']

# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = 'alabaster'
html_static_path = ['_static']

if os.environ.get('READTHEDOCS', None) == 'True':
     subprocess.call('cd ..; doxygen', shell=True)
