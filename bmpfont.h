#ifndef BMPFONT_H_INCLUDED
#define BMPFONT_H_INCLUDED
GLubyte space[] =
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
GLubyte letters[][13] = {
    {0x00, 0x00, 0xc3, 0xc3, 0xc3, 0xc3, 0xff, 0xc3, 0xc3, 0xc3, 0x66, 0x3c, 0x18},
    {0x00, 0x00, 0xfe, 0xc7, 0xc3, 0xc3, 0xc7, 0xfe, 0xc7, 0xc3, 0xc3, 0xc7, 0xfe},
    {0x00, 0x00, 0x7e, 0xe7, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xe7, 0x7e},
    {0x00, 0x00, 0xfc, 0xce, 0xc7, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc7, 0xce, 0xfc},
    {0x00, 0x00, 0xff, 0xc0, 0xc0, 0xc0, 0xc0, 0xfc, 0xc0, 0xc0, 0xc0, 0xc0, 0xff},
    {0x00, 0x00, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xfc, 0xc0, 0xc0, 0xc0, 0xff},
    {0x00, 0x00, 0x7e, 0xe7, 0xc3, 0xc3, 0xcf, 0xc0, 0xc0, 0xc0, 0xc0, 0xe7, 0x7e},
    {0x00, 0x00, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xff, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3},
    {0x00, 0x00, 0x7e, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x7e},
    {0x00, 0x00, 0x7c, 0xee, 0xc6, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06},
    {0x00, 0x00, 0xc3, 0xc6, 0xcc, 0xd8, 0xf0, 0xe0, 0xf0, 0xd8, 0xcc, 0xc6, 0xc3},
    {0x00, 0x00, 0xff, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0},
    {0x00, 0x00, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xdb, 0xff, 0xff, 0xe7, 0xc3},
    {0x00, 0x00, 0xc7, 0xc7, 0xcf, 0xcf, 0xdf, 0xdb, 0xfb, 0xf3, 0xf3, 0xe3, 0xe3},
    {0x00, 0x00, 0x7e, 0xe7, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xe7, 0x7e},
    {0x00, 0x00, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xfe, 0xc7, 0xc3, 0xc3, 0xc7, 0xfe},
    {0x00, 0x00, 0x3f, 0x6e, 0xdf, 0xdb, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0x66, 0x3c},
    {0x00, 0x00, 0xc3, 0xc6, 0xcc, 0xd8, 0xf0, 0xfe, 0xc7, 0xc3, 0xc3, 0xc7, 0xfe},
    {0x00, 0x00, 0x7e, 0xe7, 0x03, 0x03, 0x07, 0x7e, 0xe0, 0xc0, 0xc0, 0xe7, 0x7e},
    {0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0xff},
    {0x00, 0x00, 0x7e, 0xe7, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3},
    {0x00, 0x00, 0x18, 0x3c, 0x3c, 0x66, 0x66, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3},
    {0x00, 0x00, 0xc3, 0xe7, 0xff, 0xff, 0xdb, 0xdb, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3},
    {0x00, 0x00, 0xc3, 0x66, 0x66, 0x3c, 0x3c, 0x18, 0x3c, 0x3c, 0x66, 0x66, 0xc3},
    {0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3c, 0x3c, 0x66, 0x66, 0xc3},
    {0x00, 0x00, 0xff, 0xc0, 0xc0, 0x60, 0x30, 0x7e, 0x0c, 0x06, 0x03, 0x03, 0xff},
    {0x00, 0x00, 0x3c, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3c}, //0
    {0x00, 0x00, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x66, 0x36, 0x1e, 0x0e, 0x06},
    {0x00, 0x00, 0x7e, 0x7e, 0x60, 0x30, 0x18, 0x0c, 0x06, 0x02, 0x42, 0x66, 0x3c},
    {0x00, 0x00, 0x3c, 0x4e, 0x46, 0x06, 0x1e, 0x1c, 0x06, 0x06, 0x46, 0x4e, 0x3c},
    {0x00, 0x00, 0x06, 0x06, 0x06, 0x06, 0x06, 0x7e, 0x7e, 0x66, 0x66, 0x66, 0x66},
    {0x00, 0x00, 0x78, 0x7c, 0x0e, 0x06, 0x06, 0x0e, 0x7c, 0x60, 0x60, 0x7e, 0x7e},
    {0x00, 0x00, 0x3c, 0x66, 0x66, 0x66, 0x66, 0x7c, 0x60, 0x60, 0x60, 0x66, 0x3c},
    {0x00, 0x00, 0x06, 0x06, 0x06, 0x06, 0x06, 0x3e, 0x06, 0x06, 0x06, 0x7e, 0x7e},
    {0x00, 0x00, 0x3c, 0x66, 0x66, 0x66, 0x3c, 0x3c, 0x66, 0x66, 0x66, 0x66, 0x3c},
    {0x00, 0x00, 0x3c, 0x66, 0x06, 0x06, 0x06, 0x3e, 0x66, 0x66, 0x66, 0x66, 0x3c}
};

GLuint fontOffset;

void addGlyph(char ch, GLubyte* v){
   glNewList(fontOffset + ch, GL_COMPILE);
   //GLubyte v[13] = {0x00, 0x00, 0x00, 0x08, 0x2a, 0x1c, 0x7f, 0x1c, 0x2a, 0x08, 0x00, 0x00, 0x00};
   glBitmap(8, 13, 0.0, 2.0, 10.0, 0.0, v);
   glEndList();
}

void makeRasterFont(void)
{
   GLuint i, j;
   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

   fontOffset = glGenLists (128);
   for (i = 0,j = 'A'; i < 26; i++,j++) {
      glNewList(fontOffset + j, GL_COMPILE);
      glBitmap(8, 13, 0.0, 2.0, 10.0, 0.0, letters[i]);
      glEndList();
   }
   for (i = 26,j = '0'; i < 36; i++,j++) {
      glNewList(fontOffset + j, GL_COMPILE);
      glBitmap(8, 13, 0.0, 2.0, 10.0, 0.0, letters[i]);
      glEndList();
   }
   //Dot
   GLubyte dot[13] = {0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
   GLubyte colon[13] = {0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00};
   GLubyte aster[13] = {0x00, 0x00, 0x00, 0x08, 0x2a, 0x1c, 0x7f, 0x1c, 0x2a, 0x08, 0x00, 0x00, 0x00};
   GLubyte slash[13] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x7e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
   addGlyph('.', dot);
   addGlyph(':', colon);
   addGlyph('*', aster);
   addGlyph('-', slash);

   glNewList(fontOffset + ' ', GL_COMPILE);
   glBitmap(8, 13, 0.0, 2.0, 10.0, 0.0, space);
   glEndList();

}

void init(void)
{
   glShadeModel (GL_FLAT);
   makeRasterFont();
}

void printString(char *s)
{
   glPushAttrib (GL_LIST_BIT);
   glListBase(fontOffset);
   glCallLists(strlen(s), GL_UNSIGNED_BYTE, (GLubyte *) s);
   glPopAttrib ();
}



#endif // BMPFONT_H_INCLUDED
