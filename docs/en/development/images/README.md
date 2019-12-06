Although PNG images can be used for both HTML and PDF output,
SVG images are only supported in HTML output.

For PDF output you need to convert vector graphics in an SVG file to PDF.
You can do this with Inkscape: select “Print” and print the SVG to a PDF file.
Remember to check the result: my image was rotated 90° counterclockwise,
so I rotated it 90° clockwise before final printing.

A little nuance in the RST file. Instead of

    .. image:: images/image.svg

use

    .. image:: images/image.*

And the image will be selected automatically among the existing `image.svg` and `image.pdf`.
