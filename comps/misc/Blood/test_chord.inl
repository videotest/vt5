			if(i2-i1<10) continue; //хорды короче 10 точек скипаем
			if(i1+m_len-i2<10) continue;
			double area_int12 = d_area(i1,i2);
			double perim_int12 = d_perim(i1,i2);
			double s1=parea_int[i2]-parea_int[i1] - area_int12;
			double s2=parea_int[i1+m_len]-parea_int[i2] + area_int12;
			if(s<0) { s1=-s1; s2=-s2; }
			double p1=pperim_int[i2]-pperim_int[i1] - perim_int12;
			double p2=pperim_int[i1+m_len]-pperim_int[i2] + perim_int12;
			if(s1<m_fMinArea || s2<m_fMinArea || p1<10 || p2<10) continue; //не трогаем мелочь, заодно избежим деления на 0
			double rnd1 = p1*p1/(s1*3.14159*4); //roundness
			double rnd2 = p1*p1/(s1*3.14159*4);
			/*
			if(pangle[i1]>0 || pangle[i2]>0) continue; //разрез только от впадины до впадины
			if(pangle[i1]+pangle[i2]>m_fAngleThreshold) continue; //не резать от прямой до прямой и т.п.
			*/
			int a_step=int(perim_int12*m_fAngleDetectorChordCoeff)+m_lMinAngleDetector; //всегда не менее 3 пикселей
			double a1=CalcAngle(i1,a_step);
			double a2=CalcAngle(i2,a_step);
			if(a1>0 || a2>0) continue; //разрез только от впадины до впадины
			if(a1+a2>m_fAngleThreshold) continue; //не резать от прямой до прямой и т.п.

			/*
			double x1_a = (pnt(i1-a_step).x + pnt(i1+a_step).x)/2;
			double y1_a = (pnt(i1-a_step).y + pnt(i1+a_step).y)/2;
			double x2_a = (pnt(i2-a_step).x + pnt(i2+a_step).x)/2;
			double y2_a = (pnt(i2-a_step).y + pnt(i2+a_step).y)/2;
			double chord_a = _hypot(x2_a-x1_a, y2_a-y1_a);
			*/

			//double q_a = perim_int12/max(chord_a,1);
			//double q = perim_int12 / min(s1/p1, s2/p2); // длина хорды делить на характ. толщину более тонкой части
			double q = perim_int12 / sqrt(min(s1, s2)); // длина хорды делить на радиус меньшей части
			//double q = perim_int12 / max(rnd1, rnd2); // длина хорды делить на округлость меньшей части
			if(q<best_q)
			{
				best_q=q;
				best_i1=i1;
				best_i2=i2;
				best_a1=a1; best_a2=a2;
			}
