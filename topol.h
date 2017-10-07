#ifndef TOPOL_H_INCLUDED
#define TOPOL_H_INCLUDED

class binMap
{

    int ngroups;
    vector<vector<int> > column;
    vector<vector<vector<int> > > row;

    signed int firstUnmatched1(vector<int> v)
    {
        /***********************************************************

            Returns an integer with the position of the first
            unmatched 1. If none is found, the function
            returns -1.

            @v is the input vector to be searched for unmatched 1s

        ***********************************************************/

        signed int counter = 0;
        int i;
        for (i = 0; i < v.size(); i++)
        {

            if (v[i] == 1)
            {
                counter++;
                if (counter > 0)
                    return i;
            }
            else
            {
                counter--;
            }
        }
        return -1;      // no unmatched @which
    }

    signed int firstUnmatched0(vector<int> v)
    {
        /***********************************************************

            Returns an integer with the position of the first
            unmatched 0. If none is found, the function
            returns -1.

            @v is the input vector to be searched for unmatched 0s

        ***********************************************************/

        signed int counter = 0;
        int i, j;
        bool ismatched = false;
        for (i = 0; i < v.size() - 1; i++)
        {
            counter = 0;
            if (v[i] == 0)
            {
                ismatched = false;
                for (j = i + 1; j < v.size(); j++)
                {
                    if (v[j] == 0)
                    {
                        counter++;
                    }
                    else
                    {
                        counter--;
                    }
                    if (counter < 0)
                        ismatched = true;
                }
                if (!ismatched)
                    return i;
            }
        }
        if (v[v.size() - 1] == 0)
            return (v.size()-1);
        return -1;      // no unmatched @which
    }
    vector<vector<int> > getBranches(vector<int> v)
    {
        /***********************************************************

            Returns a vector containing all of the branches derived
            from the input vector. These are obtained by changing to
            1 each 0 to the right of the last 1 if the resulting
            vector has no unmatched 1s. If there are no branches,
            the return vector is empty.

            @v input binary vector

        ***********************************************************/

        int i;
        int last1=0;
        vector<int> tempV;
        vector<vector<int> > result;
        for (i = 0; i < v.size(); i++)
        {// Get last 1
            if (v[i]==1)
                last1 = i;
        }
        for (i = last1 + 1; i < v.size(); i++)
        {
            tempV = v;
            tempV[i] = 1;
            if (firstUnmatched1(tempV) == -1)
            {
                result.insert(result.end(), tempV);
            }
        }
        return result;
    }

    void fillColumn(vector<int> v)
    {
        vector<int> result;
        int position = 0;
        result = v;
        position = firstUnmatched0(result);
        column.insert(column.end(), v);
        while (position != -1)
        {
            result[position] = 1;
            column.insert(column.end(), result);
            position = firstUnmatched0(result);
        }
        row.insert(row.end(), column);
        column.clear();
    }

    void fillRow(vector<vector<int> > c)
    {
        int i;
        vector<vector<int> > tempc;
        for (i = 0; i < c.size(); i++)
        {
            fillColumn(c[i]);
            tempc = getBranches(c[i]);
            fillRow(tempc);
        }
    }

public:

    friend class borderLine;

    binMap(int n)
    {
        vector<int> currentV;
        vector<vector<int> > currentC;
        ngroups = n;
        currentV = toBin(0, n);
        fillColumn(currentV);
        currentC = getBranches(currentV);
        fillRow(currentC);
    }
    binMap()
    {}
    void textOut()
    {
        int i, j, k;
        for (i = 0; i < row.size(); i++)
        {
            for (j = 0; j < row[i].size(); j++)
            {
                for (k = 0; k < row[i][j].size(); k++)
                {
                    cout << row[i][j][k];
                }
                cout << " ";
            }
            cout << endl;
        }
    }
};


class borderLine
{
    vector<point> p;
    vector<vector<point> > bl;
    vector<vector<point> > bl_old5;
    vector<vector<point> > bl_old10;
    vector<point> circles;
    vector<point> circles_old5;
    vector<point> circles_old10;
    UINT ngroups;
    UINT startPerim;
    lCounter blCounter;
    lCounter deciderCounter;
    lCounter refreshScreen;
    vector<float> w;
    vector<rgb> colors;
    vector<point> warn;
    scale internalScale;
    int totalExpectedSurface;
    float surfRatio;
    float minSurfRatio;
    float startdt;
    timeMaster udt;
    float minratio;
    vector<char*> dataDisplay;
    float stepdt;
    bool fixCircles;

    void attention(float x, float y)
    {
        point temp;
        temp.x = x;
        temp.y = y;
        temp.radius = 1;
        warn.insert(warn.end(), temp);
    }

    void initOlds()
    {
        bl_old5 = bl;
        bl_old10 = bl;
        circles_old5 = circles;
        circles_old10 = circles;
    }

    point place(scale s, point m)
    {
        point r;
        scale tempScale;
        float offset;
        float Scale;
        if (s.ratio() <= internalScale.ratio())
        {
            offset = s.xSpan() - s.ySpan() / internalScale.ratio();
            offset /= 2;
            tempScale.minX = offset + s.minX;
            tempScale.maxX = s.maxX - offset;
            tempScale.minY = s.minY;
            tempScale.maxY = s.maxY;
        }
        else
        {
            offset = s.ySpan() - internalScale.ratio() * s.xSpan();
            offset /= 2;
            tempScale.minX = s.minX;
            tempScale.maxX = s.maxX;
            tempScale.minY = offset + s.minY;
            tempScale.maxY = s.maxY - offset;
        }
        r.x = m.x - internalScale.minX;
        r.y = m.y - internalScale.minY;
        r.x /= internalScale.xSpan();
        r.y /= internalScale.ySpan();
        r.x *= tempScale.xSpan();
        r.y *= tempScale.ySpan();
        r.x += tempScale.minX;
        r.y += tempScale.minY;
        Scale = tempScale.xSpan() / internalScale.xSpan();
        r.radius = Scale * m.radius;
        return r;
    }

    bool isInside(point p0, point p1, point target)
    {
        /**
        Returns true if target is inside the box limited
        by p0 and p1. False otherwise
        **/

        bool result = true;
        float xdiscr, ydiscr;
        xdiscr = (target.x - p0.x) * (target.x - p1.x);
        ydiscr = (target.y - p0.y) * (target.y - p1.y);
        if (xdiscr > 0 || ydiscr > 0)
        {
            result = false;
        }
        return result;
    }

    void setCircles(binMap b)
    {
        int i, j;
        int n;
        int height;
        int offset;
        float maxw = 0;
        point cpoint;
        height = b.row[0].size();
        for (i = 0; i < w.size(); i++)
        {
            if (w[i] > maxw)
                maxw = w[i];
        }
        for (i = 0; i < b.row.size(); i++)
        {
            offset = (height - b.row[i].size()) / 2;
            for (j = 0; j < b.row[i].size(); j++)
            {
                n = toInt(b.row[i][j]);
                cpoint.n = n;
                cpoint.x = 6 * i;
                cpoint.y = 6 * (j + offset - 1);
                cpoint.radius = 2 * sqrt(w[n] / maxw);
                circles.insert(circles.end(), cpoint);
            }
        }
    }


    void setScale(point p)
    {
        if (internalScale.clear == true)
        {
            internalScale.minX = p.x;
            internalScale.minY = p.y;
            internalScale.maxX = p.x;
            internalScale.maxY = p.y;
            internalScale.clear = false;
        }
        else
        {
            if (p.x < internalScale.minX)
                internalScale.minX = p.x;
            if (p.y < internalScale.minY)
                internalScale.minY = p.y;
            if (p.x > internalScale.maxX)
                internalScale.maxX = p.x;
            if (p.y > internalScale.maxY)
                internalScale.maxY = p.y;
        }
    }

    void setPoints(binMap b, int ngroup)
    {
        int i, j, counter;
        int cstart;         //placement of first and last point
        //depending on ngroup
        point cpoint;       //point storage
        point fpoint;       //first point
        int height;
        int width;
        int omyheight;      //offset
        int myheight;
        int isOne;
        int groupNumber;
        height = b.row[0].size();
        // First point
        if (ngroup < ngroups - 1)
        {
            cstart = b.ngroups - ngroup - 2;
        }
        else
        {
            cstart = ngroups - 1;
        }
        fpoint.x = -6 * (1 + cstart);
        fpoint.y = 6 * (height + 1 + cstart);
        setScale(fpoint);
        p.insert(p.end(), fpoint);
        // Variable points
        width = b.row.size();
        for (i = 0; i < width; i++)
        {
            cpoint.x = 6 * i - 2;
            // mygroup
            myheight = b.row[i].size();
            omyheight = (height - myheight) / 2;  // the diff is always even
            isOne = 1;
            counter = myheight;                   // counts the row
            if (b.row[i][counter -1][ngroup] == 1)
            {
                while (isOne == 1)
                {
                    counter--;
                    if (counter >= 0)
                    {
                        isOne = b.row[i][counter][ngroup];
                    }
                    else
                    {
                        isOne = 0;
                    }
                }
            }
            else
            {
                counter--;
            }
            cpoint.y = 6 * (omyheight + counter) - 3;
            setScale(cpoint);
            p.insert(p.end(), cpoint);
            cpoint.x = 6 * i + 2;
            setScale(cpoint);
            p.insert(p.end(), cpoint);
            // add circles
        }
        //last point
        cpoint.x = 6 * (width + 1 + cstart);
        cpoint.y = 6 * (height + 1 + cstart);
        cpoint.mass = POINT_MASS;
        setScale(cpoint);
        p.insert(p.end(), cpoint);
        initOlds();
    }

    void clearForces()
    {
        int i, j;
        //clear line point forces
        for (i = 0; i < bl.size(); i++)
        {
            for (j = 0; j < bl[i].size(); j++)
            {
                bl[i][j].fx = 0;
                bl[i][j].fy = 0;
            }
        }
        //clear circle forces
        for (i = 0; i < circles.size(); i++)
        {
            circles[i].fx = 0;
            circles[i].fy = 0;
        }
    }

    void resetOld()
    {
        int i, j;
        for (i = 0; i < bl_old5.size(); i++)
        {
            for(j = 0; j < bl_old5[i].size(); j++)
            {
                bl_old5[i][j].reset();
            }
        }
        for (i = 0; i < bl_old10.size(); i++)
        {
            for (j = 0; j < bl_old10[i].size(); j++)
            {
                bl_old10[i][j].reset();
            }
        }
        surfRatio = estSurf();
        minSurfRatio = surfRatio;
    }

    vector<point> glCircle(float x, float y, float r)
    {
        point temp;
        vector<point> result;
        float sen = 0.7071067811f;
        temp.x = x;
        temp.y = y + r;
        result.insert(result.end(), temp);
        temp.x = x + sen * r;
        temp.y = y + sen * r;
        result.insert(result.end(), temp);
        temp.x = x + r;
        temp.y = y;
        result.insert(result.end(), temp);
        temp.x = x + sen * r;
        temp.y = y - sen * r;
        result.insert(result.end(), temp);
        temp.x = x ;
        temp.y = y - r;
        result.insert(result.end(), temp);
        temp.x = x - sen * r;
        temp.y = y - sen * r;
        result.insert(result.end(), temp);
        temp.x = x - r;
        temp.y = y;
        result.insert(result.end(), temp);
        temp.x = x - sen * r;
        temp.y = y + sen * r;
        result.insert(result.end(), temp);
        return result;
    }

    point eqforce(point &p0, point &p1, float kattr = 7e-5f)
    {
        point result;
        float radius;
        float rdiff;
        float dx, dy;
        float fatt, frep;
        float d;
        radius = p0.radius + p1.radius;
        //radius *= 1.2;
        dx = p1.x - p0.x;
        dy = p1.y - p0.y;
        d = distance(p0.x, p0.y, p1.x, p1.y);
        fatt = sk *kattr* (d - radius);
        result.fx = dx * fatt;
        result.fy = dy * fatt;
        p0.fx += result.fx;
        p1.fx -= result.fx;
        p0.fy += result.fy;
        p1.fy -= result.fy;
        return result;
    }

    point spring(point &p0, point &p1, float dampen = 0, float springK = sk)
    {
        point result;
        float dx = p1.x - p0.x;
        int sx = dx / abs(dx);
        float dy = p1.y - p0.y;
        int sy = dy / abs(dy);
        result.fx = springK * dx;
        result.fy = springK * dy;
        p0.fx += result.fx - dampen * p0.vx * p0.vx * p0.vx;
        p1.fx -= result.fx + dampen * p1.vx * p1.vx * p1.vx;
        p0.fy += result.fy - dampen * p0.vy * p0.vy * p0.vy;
        p1.fy -= result.fy + dampen * p1.vy * p1.vy * p1.vy;
        return result;
    }

    /*point contact(point &p0, point &p1, float hardness = 5e3f){
        point result;
        point zero;
        bool isContact;
        float d;
        float dx;
        float dy;
        float kx;
        float ky;
        float dsq;
        float radius;
        float ratio;
        UINT np = bl[0].size();
        hardness /= np;
        zero.fx = 0;
        zero.fy = 0;
        dx = p1.x - p0.x;
        dy = p1.y - p0.y;
        dsq = dx*dx + dy*dy;
        if (dsq == 0) dsq = 1e-10f;
        isContact = false;
        radius = p0.radius + p1.radius;
        //check contact
        d = distance(p0.x, p0.y, p1.x, p1.y);
        //if (d < radius*0.7f) d = radius*0.9f;
        if (d < radius){
            ratio = (d/radius);
            kx = dx * (-1.0f + ratio);
            ky = dy * (-1.0f + ratio);
            result.fx = hardness * sk * kx;
            result.fy = hardness * sk * ky;
            p0.fx += result.fx;
            p0.fy += result.fy;
            p1.fx -= result.fx;
            p1.fy -= result.fy;
            return result;
        } else {
            return zero;
        }
    }*/

    point contact(point &p0, point &p1, float hardness = 5e1f)
    {
        point result;
        point zero;
        float d;
        float dx;
        float dy;
        float dsq;
        float radius;
        float sc;
        float fc, fc1, fc0;
        float vc, vc1, vc0;
        float vx, fx;
        float vy, fy;
        float v0, f0;
        float v1, f1;
        float m0, m1;
        float kx, ky;
        float dr;
        float ratio;
        zero.fx = 0;
        zero.fy = 0;
        dx = p1.x - p0.x;
        dy = p1.y - p0.y;
        m0 = p0.mass;
        m1 = p1.mass;
        radius = p0.radius + p1.radius;

        //check contact
        d = distance(p0.x, p0.y, p1.x, p1.y);

        vx = p1.vx - p0.vx;
        vy = p1.vy - p0.vy;
        vc = dx*vx + dy*vy;


        fx = p1.fx - p0.fx;
        fy = p1.fy - p0.fy;
        fc = dx*fx + dy*fy;

        if (d <= radius)
        {
            dr = radius / d;
            if (d <= radius){
                ratio = d/(radius);
                kx = dx * (-1.0f + ratio);
                ky = dy * (-1.0f + ratio);
                result.fx = hardness * sk * kx;
                result.fy = hardness * sk * ky;
                p0.fx += result.fx;
                p0.fy += result.fy;
                p1.fx -= result.fx;
                p1.fy -= result.fy;
//                attention(p0.x, p0.y, 1, 1);
            }
            return result;
        }
        else
        {
            return zero;
        }
    }

    point rope(point &circ, point &p0, point &p1)
    {
        float dist;
        float a, b;
        float d;
        point result;
        point zero;
        point virt;
        float d0, d1;
        bool isContact;
        a = (p1.y - p0.y)/(p1.x - p0.x);
        b = p0.y - a * p0.x;
        virt.x = ((circ.x + a*circ.y - a * b)/(a*a + 1));
        virt.y = (a * circ.x + a*a*circ.y + b)/(a*a + 1);
        d0 = distance(p0.x, p0.y, virt.x, virt.y) /
             distance(p0.x, p0.y, p1.x, p1.y);
        d1 = distance(p1.x, p1.y, virt.x, virt.y) /
             distance(p0.x, p0.y, p1.x, p1.y);
        virt.vx = p0.vx + (p1.vx-p0.vx) * d0;
        virt.fx = p0.fx + (p1.fx-p0.fx) * d0;
        virt.vy = p0.vy - (p1.vy-p0.vy) * d0;
        virt.fy = p0.fy - (p1.fy-p0.fy) * d0;
        virt.mass = p0.mass;
        d = distance(circ.x, circ.y, virt.x, virt.y);
        if (d < circ.radius && isInside(p0, p1, virt))
        {
            result = contact(circ, virt);
            p0.fx = -d1 * result.fx;
            p0.fy = -d1 * result.fy;
            p1.fx = -d0 * result.fx;
            p1.fy = -d0 * result.fy;
            //attention(p0.x, p0.y, p0.x+1e-3*p0.fx, p0.y+1e-3*p0.fy);
            //attention(p1.x, p1.y, p1.x+1e-3*p1.fx, p1.y+1e-3*p1.fy);
            //Sleep(100);
            return result;
        }
        else
        {
            return zero;
        }
    }

    void setForces1()
    {
        int i, j, k;
        int cycle;
        int size;
        point f;
        //line points
        /*******/
        float damp = sk / 100;
        for (i = 0; i < bl.size(); i++)
        {
            //first point
            size = bl[i].size();
            f = spring(bl[i][0], bl[i][size-1], damp);
            //rest of the points
            for (j = 1; j < size; j++)
            {
                f = spring(bl[i][j], bl[i][j-1], damp);
            }
        }

    }

    void setContacts()
    {
        int i, j, k;
        int cycle;
        int size;
        point f;

        //contact forces
        /*******/
        //line-circle

        for (k = 0; k < bl.size(); k++)
        {
            size = bl[k].size();
            for (j = 0; j < circles.size(); j++)
            {
                if (circles[j].radius > 0)
                {
                    /******first line with circles****/
                    f = rope(circles[j], bl[k][size-1], bl[k][0]);
                    /******/
                    for (i = 0; i < bl[k].size(); i++)
                    {
                        /****rest of the lines with circles**/
                        if (i > 0)
                        {
                            f = rope(circles[j], bl[k][i-1], bl[k][i]);
                        }
                        /****line points with circles**/
                        f = contact(circles[j], bl[k][i]);
                        /******/
                    }

                }
            }
        }
        /*******/
        //circle-circle
        for (i = 0; i < circles.size() - 1; i++)
        {
            for (j = i + 1; j < circles.size(); j++)
            {
                //f = eqforce(circles[i], circles[j]);
                f = contact(circles[i], circles[j]);
            }
        }
        /*******/
        if (fixCircles){
          for (i = 0; i < circles.size(); i++){
            circles[i].fx = 0;
            circles[i].fy = 0;
            circles[i].vx = 0;
            circles[i].vy = 0;
          }
        }
    }
    void setForces2()
    {
        int i, j, k;
        int cycle;
        int size;
        point f;
        //cout << ".";
        //line points
        /*******/
        for (i = 0; i < bl.size(); i++)
        {
            //first point
            size = bl[i].size();
            f = spring(bl[i][0], bl[i][size-1]);
            //rest of the points
            for (j = 1; j < size; j++)
            {
                f = spring(bl[i][j], bl[i][j-1]);
            }
        }
    }

    void setForces3()
    {
        int i, j, k;
        int i1, i2;
        point f;
        int size;
        point previous;
        point next;
        float Scale;
        float storeR = 0;
        Scale = 0.02;
        string temp;
        //fixCircles = true;

        float dampen = sk / 10;
        /*******/
        //line-circle
        for (k = 0; k < bl.size(); k++)
        {
            size = bl[k].size();
            for (j = 0; j < circles.size(); j++)
            {
                for (i = 0; i < bl[k].size(); i++)
                {
                    if (circles[j].radius > 0)
                    {
                        bl[k][i].radius = Scale * k;
                        storeR = circles[j].radius;
                        circles[j].radius -= 2*bl[k][i].radius;
                        f = contact(circles[j], bl[k][i]);
                        f = eqforce(circles[j], bl[k][i], 1e-4f);
                        circles[j].radius = storeR;
                        circles[j].fx = 0;
                        circles[j].fy = 0;
                        circles[j].vx = 0;
                        circles[j].vy = 0;
                        if (i > 0)
                        {
                            i1 = i - 1;
                        }
                        else
                        {
                            i1 = bl[k].size() - 1;
                        }
                        if (i < bl[k].size() - 1)
                        {
                            i2 = i + 1;
                        }
                        else
                        {
                            i1 = 0;
                        }
                        f = spring(bl[k][i1], bl[k][i], dampen);
                        f = spring(bl[k][i], bl[k][i2], dampen);

                    }
                }
            }
        }
        /** Circle-circle
        for (i = 0; i < circles.size() - 1; i++){
            for (j = i + 1; j < circles.size(); j++){
              f = eqforce(circles[i], circles[j]);
            }
        } **/
    }

    void limitForce(point &p, float maxf)
    {
        float f;
        f = p.fx * p.fx + p.fy * p.fy;
        if (f > maxf*maxf)
        {
            f = sqrt(f);
            p.fx *= maxf/f;
            p.fy *= maxf/f;
        }
    }


    void solve(bool resetVelocity = false)
    {
        int i, j;
        float kb;
        float maxf;

        if (resetVelocity)
        {
            maxf = 5e10f;
            kb = 1e2f;//1e2f;
        }
        else
        {
            maxf = 5e30f;
            kb = 1e2f;
        }


        //Init the scale for the new frame
        internalScale.clear = true;
        setContacts();
        updPos(kb, resetVelocity);
        clearForces();
        surfRatio = estSurf();
        if (minSurfRatio == 0){
          minSurfRatio = surfRatio;
        }
        if(minSurfRatio > surfRatio){
          minSurfRatio = surfRatio;
        }
//Show dt
        char* dsp = (char*) calloc(100, sizeof(char));
        sprintf(dsp, "DT: %.8f", dt);
        dataDisplay.insert(dataDisplay.end(), dsp);

        char* sur = (char*) calloc(100, sizeof(char));
        sprintf(sur, "SURF.: %.4f", surfRatio);
        dataDisplay.insert(dataDisplay.end(), sur);

        char* msur = (char*) calloc(100, sizeof(char));
        sprintf(msur, "MINSURF.: %.4f", minSurfRatio);
        dataDisplay.insert(dataDisplay.end(), msur);
//
        if (checkTopol() || surfRatio > (2 * minSurfRatio))
        {
            bl = bl_old10;
            circles = circles_old10;
            udt.report(dt);
            dt *= stepdt;
            deciderCounter = 0;
        }
        else if (blCounter == 0)
        {
            bl_old10 = bl_old5;
            bl_old5 = bl;
            resetOld();
            circles_old10 = circles_old5;
            circles_old5 = circles;
        }
        else if (deciderCounter.isMax())
        {
            if (dt/stepdt < udt.unstabledt())
            {
                dt /= stepdt;
            }
        }
        blCounter++;
        deciderCounter++;

    }

    void updPos(float kb, bool resetVelocity)
    {
        int i, j;
        //Apply each force to each point
        for (i = 0; i < bl.size(); i++)
        {
            for (j = 0; j < bl[i].size(); j++)
            {
                bl[i][j].fx -= kb * bl[i][j].vx;
                bl[i][j].fy -= kb * bl[i][j].vy;
                //Limit force to avoid artifacts
                //limitForce(bl[i][j], maxf);
                bl[i][j].vx += bl[i][j].fx * dt / bl[i][j].mass;
                bl[i][j].vy += bl[i][j].fy * dt / bl[i][j].mass;
                bl[i][j].x += bl[i][j].vx * dt;
                bl[i][j].y += bl[i][j].vy * dt;
                /*******/
                //attention(bl[i][j].x, bl[i][j].y, 0.1);
                /*******/
                //limitVel(bl[i][j], maxv);
                //Prepare the scale for the new frame
                setScale(bl[i][j]);

                if (resetVelocity)
                {
                    bl[i][j].vx = 0;
                    bl[i][j].vy = 0;
                }
            }

            //float p = perimeter(bl[i], true);
            //char* t = (char*) calloc(100, sizeof(char));
            //sprintf(t, "P%u: %.4f", i, p);
            //dataDisplay.insert(dataDisplay.end(), t);
        }

        for (i = 0; i < circles.size(); i++)
        {

            /*******
            limitForce(circles[i], maxf);
            /*******/
            circles[i].fx -= kb * circles[i].vx;
            circles[i].fy -= kb * circles[i].vy;
            //if (circles[i].vx){
            //    temp(1e5*circles[i].vx);
            //}
            circles[i].vx += circles[i].fx * dt / CIRCLE_MASS;
            circles[i].vy += circles[i].fy * dt / CIRCLE_MASS;
            //limitVel(circles[i], maxv);
            circles[i].x += circles[i].vx * dt;
            circles[i].y += circles[i].vy * dt;
            if (resetVelocity)
            {
                circles[i].vx = 0;
                circles[i].vy = 0;
            }
        }
    }

    float estSurf(int nPoints = 100){
      int i;
      float perc;
      float tsurf = (float) (internalScale.xSpan() * internalScale.ySpan());
      tsurf /= (float) totalExpectedSurface;
      return tsurf;
    }

    bool isTopolCorrect(point P, vector<int> belong){
      int j, k;
      int lastPoint;
      bool mustBeIn;  //Must the circle be inside the curve?
      bool isIn;      //Is the circle inside the curve?
      bool ch;        //Must isIn change?
      bool isAbove;   //Is the curve point above the circle center?
      bool isRight;   //Is the curve point to the right of the circle center?
      float a, b;
      point previousPoint;
      point testPoint;
      for (j = 0; j < bl.size(); j++)
      {
          lastPoint = bl[j].size()-1;
          isIn = false;
          mustBeIn = (belong[j] == 1);
          isAbove = (bl[j][lastPoint].y > P.y);
          isRight = (bl[j][lastPoint].x > P.x);
          for (k = 0; k < bl[j].size(); k++)
          {
              ch = (bl[j][k].y > P.y);
              ch = (ch ^ isAbove);
              if (ch)
              {    /* Lines cross */
                  isAbove = !isAbove;
                  if (bl[j][k].x > P.x && isRight)
                  { /* Not an extreme point */
                      isIn = !isIn;
                  }
                  else if (bl[j][k].x > P.x ^ isRight)
                  { /*Extreme point */
                      if (k > 0)
                      {
                          previousPoint = bl[j][k - 1];
                      }
                      else
                      {
                          previousPoint = bl[j][lastPoint];
                      }
                      a = (bl[j][k].y - previousPoint.y)/
                          (bl[j][k].x - previousPoint.x);
                      b = bl[j][k].y - a * bl[j][k].x;
                      testPoint.y = P.y;
                      testPoint.x = (testPoint.y - b) / a;
                      if (testPoint.x > P.x
                              && isInside(previousPoint, bl[j][k], testPoint))
                      {
                          isIn = !isIn;
                      }
                  }
              }   /* Lines cross */
              isRight  = (bl[j][k].x > P.x);
          }
          if (isIn ^ mustBeIn)
          {
              //attention(circles[i].x, circles[i].y, 2);
              //Sleep(1000);
              return true;
          }
      }
      return false;
    }

    bool checkTopol()
    {
      int i;
      for (i = 0; i < circles.size(); i++)
      {
          if (circles[i].radius > 0)
          { /* Circle has radius */
              vector<int> belong = toBin(circles[i].n, bl.size()); // This might be taken out and calculated only once. Does not take long, though
              bool result = isTopolCorrect(circles[i], belong);
              if (result){
                return result;
              }
          } /* Circle has radius */
      }
      return false;
    }

    void wlimit(){
      int i;
      float wmax = 0;
      for (i = 0; i < w.size(); i++){
        if (w[i] > wmax) wmax = w[i];
      }
      for (i = 0; i < w.size(); i++){
        if (w[i] > 0 && w[i] < (wmax * minratio)) w[i] = minratio * wmax;
      }
    }

public:

    borderLine(binMap b, vector<float> tw)
    {
        int i;
        minratio = 0.05f;
        fixCircles = false;
        srand(time(0));
        w = tw;         //keep a copy of the weights
        wlimit();


        int height;     //number of rows in the first column
        ngroups = b.ngroups;
        height = b.row[0].size();

        //init circles
        setCircles(b);

        totalExpectedSurface = 0;
        for (i = 0; i < w.size(); i++){
          totalExpectedSurface += circles[i].radius * circles[i].radius;
        }

        //init counters
        blCounter.setLimits(0, 30u);
        deciderCounter.setLimits(0, 300u);
        refreshScreen.setLimits(1, 10);

        //init internal scale
        internalScale.clear = true;

        //init time parameters
        startdt = dt;
        udt.init(startdt);
        stepdt = 0.60;

        //init points
        for (i = 0; i < ngroups; i++)
        {
            p.clear();
            setPoints(b, i);
            bl.insert(bl.end(), p);
        }
        startPerim = perimeter(bl[0]);
        UINT np = (UINT) (0.5f * (float) startPerim);
        interpolate(np);

        //init colors
        for (i = 0; i <= 0x00FFFFFF; i = i + (int)0x00FFFFFF/ngroups)
        {
            colors.insert(colors.end(),toRGB(i, 1));
        }
        minSurfRatio = 0;
    }



    fileText toPS()
    {
        fileText pstext;
        char temp[512];
        string tst;
        int tsize;
        int i, j;
        point pstemp;
        scale ps;
        ps.minX = 72.0f;
        ps.minY = 72.0f;
        ps.maxX = 551.0f;
        ps.maxY = 721.0f;

        // Postscript header
        pstext.addLine(" ");
        pstext.addLine("%!PS-Adobe-2.0");
        pstext.addLine(" ");
        pstext.addLine("/scale 1 def");
        pstext.addLine("/ury 0  def");
        pstext.addLine("/urx 0  def");
        pstext.addLine("/lly 900  def");
        pstext.addLine("/llx 900  def");
        pstext.addLine(" ");
        pstext.addLine("/showline{     %draws a line from an array of points");
        pstext.addLine("topath          %stack: point_array. rgb_color");
        pstext.addLine("setrgbcolor");
        pstext.addLine("1 setlinejoin");
        pstext.addLine("stroke");
        pstext.addLine("} def");
        pstext.addLine(" ");
        pstext.addLine("/topath{        %makes a path from an array of points");
        pstext.addLine("/tmat exch def  ");
        pstext.addLine("newpath");
        pstext.addLine("tmat 0 get scale mul tmat 1 get scale mul moveto");
        pstext.addLine("2 2 tmat length 1 sub");
        pstext.addLine("{");
        pstext.addLine("dup");
        pstext.addLine("tmat exch get");
        pstext.addLine("scale mul /x exch def");
        pstext.addLine("1 add");
        pstext.addLine("tmat exch get");
        pstext.addLine("scale mul /y exch def");
        pstext.addLine("x y lineto");
        pstext.addLine("}for");
        pstext.addLine("closepath");
        pstext.addLine("}def");
        pstext.addLine(" ");
        pstext.addLine("/showp{                %draws points from an array");
        pstext.addLine("/tmat exch def         %stack: point_array. rgb_color");
        pstext.addLine("/b exch def");
        pstext.addLine("/g exch def");
        pstext.addLine("/r exch def");
        pstext.addLine("0 2 tmat length 1 sub");
        pstext.addLine("{");
        pstext.addLine("dup");
        pstext.addLine("tmat exch get");
        pstext.addLine("scale mul /x exch def");
        pstext.addLine("1 add");
        pstext.addLine("tmat exch get");
        pstext.addLine("scale mul neg /y exch def");
        pstext.addLine("newpath");
        pstext.addLine("x y 1 0 360 arc");
        pstext.addLine("r g b setrgbcolor");
        pstext.addLine("stroke");
        pstext.addLine("}for");
        pstext.addLine("}def");
        pstext.addLine(" ");
        pstext.addLine("/patternshape{");
        pstext.addLine("/size 0.5 def");
        pstext.addLine("moveto");
        pstext.addLine("size 2 div size 2 div rmoveto");
        pstext.addLine("0 size neg rlineto");
        pstext.addLine("size neg 0 rlineto");
        pstext.addLine("0 size rlineto");
        pstext.addLine("size 0 rlineto");
        pstext.addLine("} def");
        pstext.addLine("");
        pstext.addLine("/colorpattern{");
        pstext.addLine("/offsety exch def");
        pstext.addLine("/offsetx exch def");
        pstext.addLine("/b exch def");
        pstext.addLine("/g exch def");
        pstext.addLine("/r exch def");
        char* st = (char*) calloc(100, sizeof(char));
        sprintf(st, "/step %u def", ngroups);
        pstext.addLine(st);
        free(st);
        pstext.addLine("offsetx llx add step urx offsetx add{");
        pstext.addLine("/x exch def");
        pstext.addLine(" offsety lly add step ury offsety add{");
        pstext.addLine(" /y exch def");
        pstext.addLine("  newpath");
        pstext.addLine("  x y patternshape");
        pstext.addLine("  r g b setrgbcolor");
        pstext.addLine("  fill");
        pstext.addLine(" } for");
        pstext.addLine("} for");
        pstext.addLine("} def");
        pstext.addLine("");
        pstext.addLine("");
        pstext.addLine("");
        pstext.addLine("/minmax{");
        pstext.addLine("/tmat exch def");
        pstext.addLine("");
        pstext.addLine("0 2 tmat length 1 sub");
        pstext.addLine("{");
        pstext.addLine(" dup");
        pstext.addLine(" tmat exch get");
        pstext.addLine(" scale mul /x exch def");
        pstext.addLine(" 1 add");
        pstext.addLine(" tmat exch get");
        pstext.addLine(" scale mul /y exch def");
        pstext.addLine(" x urx gt{");
        pstext.addLine(" /urx x def");
        pstext.addLine(" }if");
        pstext.addLine(" x llx lt{");
        pstext.addLine(" /llx x def");
        pstext.addLine(" }if");
        pstext.addLine(" y ury gt{");
        pstext.addLine(" /ury y def");
        pstext.addLine(" }if");
        pstext.addLine(" y lly lt{");
        pstext.addLine(" /lly y def");
        pstext.addLine(" }if");
        pstext.addLine("}for");
        pstext.addLine("}def");

        // Arrays
        for (i = 0; i < ngroups; i++)
        {
            pstext.addLine("[");
            for (j = 0; j < bl[i].size(); j++)
            {
                pstemp = place(ps, bl[i][j]);
                tsize = sprintf(temp, "%f %f", pstemp.x, pstemp.y);
                tst = temp;
                pstext.addLine(tst);
            }
            tsize = sprintf(temp, "]");
            tst = temp;
            pstext.addLine(tst);
            tsize = sprintf(temp, "/set%d exch def", i + 1);
            tst = temp;
            pstext.addLine(tst);
            tsize = sprintf(temp, "set%d minmax", i + 1);
            tst = temp;
            pstext.addLine(tst);
            pstext.addLine(" ");
        }
        pstext.addLine(" ");

        // Draw sets
        pstext.addLine("%Begin program");
        pstext.addLine(" ");

        UINT offset = 0;
        for (i = 0; i < ngroups; i++)
        {
            pstext.addLine("gsave");
            tsize = sprintf(temp, "set%d topath", i+1);
            tst = temp;
            pstext.addLine(tst);
            pstext.addLine("clip");
            tsize = sprintf(temp, "%f %f %f %u %u colorpattern", colors[i].red,
                            colors[i].green, colors[i].blue, offset, offset);
            tst = temp;
            pstext.addLine(tst);
            pstext.addLine("grestore");
            offset++;
        }
        pstext.addLine(" ");

        for (i = 0; i < ngroups; i++)
        {
            tsize = sprintf(temp, "%f %f %f set%d showline", colors[i].red,
                            colors[i].green, colors[i].blue, i+1);
            tst = temp;
            pstext.addLine(tst);
        }
        pstext.addLine(" ");

        /****Draw circles*/
        for (i = 0; i < circles.size(); i++){
            pstemp = place(ps, circles[i]);
            pstext.addLine("newpath");
            tsize = sprintf(temp, "%f %f %f 0 360 arc", pstemp.x,
                            pstemp.y, pstemp.radius);
            tst = temp;
            pstext.addLine(tst);
            pstext.addLine("0.2 setlinewidth");
            pstext.addLine("1 0 0 setrgbcolor");
            pstext.addLine("stroke");
        }
        /*****/
        pstext.addLine("showpage");
        return pstext;
    }

    void interpolate(int npoints)
    {
        int i, j, k;
        float dx, dy;
        float perim;
        float segment;
        int interpoints;
        point startPoint;
        point endPoint;
        point tempPoint;
        vector<point> tempv;
        vector<vector<point> > tempbl;
        for (i = 0; i < bl.size(); i++)
        {
            perim = perimeter(bl[i], true);
            startPoint = bl[i][bl[i].size()-1];
            for (j = 0; j < bl[i].size(); j++)
            {
                endPoint = bl[i][j];
                segment = distance(startPoint.x, startPoint.y,
                                   endPoint.x, endPoint.y);
                interpoints = (int)(segment * npoints / perim);
                if (interpoints == 0)
                    interpoints = 1;
                dx = (float)(endPoint.x - startPoint.x);
                dx = dx / interpoints;
                dy = (float)(endPoint.y - startPoint.y);
                dy = dy / interpoints;
                for (k = 0; k < interpoints; k++)
                {
                    tempPoint.x = startPoint.x + (k * dx);
                    tempPoint.y = startPoint.y + (k * dy);
                    tempPoint.mass = POINT_MASS;
                    tempv.insert(tempv.end(), tempPoint);
                }
                startPoint = endPoint;
            }
            tempbl.insert(tempbl.end(), tempv);
            tempv.clear();
        }
        bl.clear();
        bl = tempbl;
        initOlds();
    }

    void simulate(int ncycles)
    {
        int i, j, k;
        int cycle;
        int size;
        UINT it1 = 1e4;
        UINT it2 = 1e3;
        point minP;
        point maxP;
        printf("Starting...\n");
        for (i = 0; i < it1; i++){
          setForces1();
          solve();
        }
        printf("Refining...\n");
        interpolate(700);
        for (i = 0; i < it2; i++){
          setForces3();
          solve();
        }
    }

};


#endif // TOPOL_H_INCLUDED
