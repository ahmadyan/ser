#if 0
/*
				//Algorithm 2: Averajangool
				
				// for each DFFs, we calcualte the LP * PP.
				for (int ff_ind=0; ff_ind<dff_num; ff_ind++){					
					gateptr = fadjlist_ptr[dfflist_ptr[ff_ind]->q];
					list_size = gateptr->event_list.size();
					//cout << ">>>>>>>>>>>>>>>>>>>>>>>>..DFF= "<< ff_ind << " " << gateptr->event_list.size()-1<< endl ;
					float let_a=1, let_an=1;

					tpsum = 0;
					tsum = 0;
					float tsum_a = 0, tsum_an=0;
					float tpsum_a=0, tpsum_an=0;

					for(int i=0; i<gateptr->event_list.size()-1;i++){
						if( gateptr->event_list[i]->qd > 0 ){
							for(int j=i+1;j<gateptr->event_list.size()-1;j++){
								if( gateptr->event_list[j]->qdn > 0 ){// we found a wave
									float no_tran_pre=1, no_tran_post=1;
									float elpp_a = 1;
									//for(int k=j+1; k<gateptr->event_list.size();k++){
									//	no_tran_post *= ( gateptr->event_list[k]->q0 + gateptr->event_list[k]->q1 );
									//}
									//for(int k=0; k<i;k++){
									//	no_tran_pre *= ( gateptr->event_list[k]->q0 + gateptr->event_list[k]->q1 );
									//}
									//elpp_a = no_tran_pre * gateptr->event_list[i]->qd * gateptr->event_list[j]->qdn *  no_tran_post ;
									elpp_a = gateptr->event_list[i]->qd ;
									float wave_duration =  gateptr->event_list[j+1]->time - gateptr->event_list[i+1]->time ;
									float lp_a = (float)(tsu+th+wave_duration)/(float)tperiod ;
									lp_a=(lp_a>1?1:lp_a);
									let_a = let_a * (1-elpp_a*lp_a);
									//let_a = let_a * (1-elpp_a);
									//let_a = let_a * (1-elpp_a*lp_a);
									//let_a += elpp_a*lp_a  ;
									//let_a = 1-let_a ;
									//latch_prop += elpp_a*lp_a ;
									//cout << endl << endl << endl ;
									//cout << "let_a= " << let_a  <<  "   elpp_a=" << elpp_a <<  "   lp_a=" << lp_a << "   -  " <<(1-elpp_a*lp_a) << endl ;;
									//cout << i << " " << j << ">> no_tran_pre=" << no_tran_pre << " no_tran_post=" << no_tran_post << "    elpp_a=" << elpp_a << endl ;
									//cout << gateptr->event_list[i]->qd << " " << gateptr->event_list[j]->qdn << endl ;
									//cout << " wave_duration=" << wave_duration << " _____ lp_a=" << lp_a << endl ;
									//cout << tsu << " "<< th << " "<< wave_duration<< " " << tperiod << endl ;
									//cout << " gateptr->event_list[i+1]->time=" << gateptr->event_list[i+1]->time << "    gateptr->event_list[j+1]->time=" << gateptr->event_list[j+1]->time << endl ; 
								}
							}
						}
						//let_a=1-let_a;
						if( gateptr->event_list[i]->qdn > 0  ){
							for(int j=i+1;j<gateptr->event_list.size()-1;j++){
								if( gateptr->event_list[j]->qd > 0 ){// we found a wave
									float no_tran_pre=1, no_tran_post=1;
									float elpp_an = 1;
									//for(int k=j+1; k<gateptr->event_list.size();k++){
									//	no_tran_post *= ( gateptr->event_list[k]->q0 + gateptr->event_list[k]->q1 );
									//}
									//for(int k=0; k<i;k++){
									//	no_tran_pre *= ( gateptr->event_list[k]->q0 + gateptr->event_list[k]->q1 );
									//}
									//elpp_an = no_tran_pre * gateptr->event_list[i]->qdn * gateptr->event_list[j]->qd *  no_tran_post ;
									elpp_an = gateptr->event_list[i]->qdn ;
									float wave_duration =  gateptr->event_list[j+1]->time - gateptr->event_list[i+1]->time ;
									float lp_an = (float)(tsu+th+wave_duration)/(float)tperiod ;
									lp_an=(lp_an>1?1:lp_an);
									let_an = let_an * (1-elpp_an*lp_an);
									//let_an = let_an * (1-elpp_an);
									//latch_prop += elpp_an*lp_an ;
									//let_an += elpp_an*lp_an ;
									//let_an = 1-let_an ;
									//cout << endl << endl << endl ;
									//cout << "let_an= " << let_an  <<  "   elpp_an=" << elpp_an <<  "   lp_an=" << lp_an << "   -  " <<(1-elpp_an*lp_an) << endl ;;
									//cout << i << " " << j << ">> no_tran_pre=" << no_tran_pre << " no_tran_post=" << no_tran_post << "    elpp_an=" << elpp_an << endl ;
									//cout << gateptr->event_list[i]->qdn << " " << gateptr->event_list[j]->qd << endl ;
									//cout << " wave_duration=" << wave_duration << " _____ lp_an=" << lp_an << endl ;
									//cout << tsu << " "<< th << " "<< wave_duration<< " " << tperiod << endl ;
									//cout << " gateptr->event_list[i+1]->time=" << gateptr->event_list[i+1]->time << "    gateptr->event_list[j+1]->time=" << gateptr->event_list[j+1]->time << endl ; 
								}
							}
						}


					}

					gptr1 = fadjlist_ptr[dfflist_ptr[ff_ind]->d];
					gptr2 = fadjlist_ptr[dfflist_ptr[ff_ind]->q];
					gptr2->p0 = gptr1->p0;
					gptr2->p1 = gptr1->p1;
					//gptr2->pd  = (let_a==0)? gptr1->pd :1-let_a ;
					//gptr2->pdn = (let_an==0)?gptr1->pdn : 1-let_an;
					gptr2->pd = 1-let_a ;//gptr1->pd;
					gptr2->pdn = 1-let_an ; //gptr1->pdn;



					tpsum = 0;
					tsum = 0;
					tsum_a = 0; tsum_an=0;
					tpsum_a=0; tpsum_an=0;

					for (k=0; k<gateptr->event_list.size()-1; k++){
						tk = gateptr->event_list[k+1]->time - gateptr->event_list[k]->time;

						float x = 0 ; 
						bool found=false;
						for(int j=gateptr->event_list.size()-2;(j>k)&&(found==false);j--){
							if (gateptr->event_list[j]->qdn > 0 ){
								x=gateptr->event_list[j]->qdn;
								found=true;
							}
						}
						tpsum_a += tk * gateptr->event_list[k]->qd * x;
						found=false; x=0;
						for(int j=gateptr->event_list.size()-2;(j>k)&&(found==false);j--){
							if (gateptr->event_list[j]->qd > 0 ){
								x=gateptr->event_list[j]->qd;
								found=true;
							}
						}
						tpsum_an += tk * gateptr->event_list[k]->qdn *x ;
						//tpsum += tk * (gateptr->event_list[k]->pd+gateptr->event_list[k]->pdn);
						if(gateptr->event_list[k]->qd>0) tsum_a += tk ; 
						if(gateptr->event_list[k]->qdn>0) tsum_an+= tk ;
						tsum += tk;
					}
					if( tsum_a != 0) let_a *= 1- ((float)tpsum_a+(tsu+th)*(float)tpsum_a/(float)tsum_a)/(float)tperiod;
					if( tsum_an != 0) let_an *= 1- ((float)tpsum_an+(tsu+th)*(float)tpsum_an/(float)tsum_an)/(float)tperiod;
					//	if (tsum != 0){
					//		let_a *= 1- ((float)tpsum_a+(tsu+th)*(float)tpsum_a/(float)tsum_a)/(float)tperiod;
					//		let_an *= 1- ((float)tpsum_an+(tsu+th)*(float)tpsum_an/(float)tsum_an)/(float)tperiod;

					//let_a *= 1- ((float)tpsum_a+(tsu+th)*(float)tpsum_a/(float)tsum)/(float)tperiod;
					//let_an *= 1- ((float)tpsum_an+(tsu+th)*(float)tpsum_an/(float)tsum)/(float)tperiod;
					//sys_failure *= 1- ((float)tpsum+(tsu+th)*(float)tpsum/(float)tsum)/(float)tperiod;
					//	}

					gptr1 = fadjlist_ptr[dfflist_ptr[ff_ind]->d];
					gptr2 = fadjlist_ptr[dfflist_ptr[ff_ind]->q];
					gptr2->p0 = gptr1->p0;
					gptr2->p1 = gptr1->p1;
					//gptr2->pd  = (let_a==0)? gptr1->pd :1-let_a ;
					//gptr2->pdn = (let_an==0)?gptr1->pdn : 1-let_an;
					gptr2->pd = (1-let_a)+(gptr2->pd/2) ;//gptr1->pd;
					gptr2->pdn = (1-let_an)+(gptr2->pd/2) ; //gptr1->pdn;

					if(gptr2->pd<0)gptr2->pd=0;
					if(gptr2->pd>1)gptr2->pd=1;
					if(gptr2->pdn<0)gptr2->pdn=0;
					if(gptr2->pdn>1)gptr2->pdn=1;

					cout << " * " << ff_ind << " " <<  gptr2->pd << " " << gptr2->pdn << endl ;

				}//for (ff_ind=0; ....

				*/
				/*
				// Algorithm 3: DSN2010 Alg. Wave flopping!
				float lp=1;
				// for each DFFs, we calcualte the LP * PP.
				for (int ff_ind=0; ff_ind<dff_num; ff_ind++){					
					gateptr = fadjlist_ptr[dfflist_ptr[ff_ind]->q];
					list_size = gateptr->event_list.size();
					//cout << ">>>>>>>>>>>>>>>>>>>>>>>>..DFF= "<< ff_ind << " " << gateptr->event_list.size()-1<< endl ;
					float let_a=1, let_an=1;
					//float let_a=0, let_an=0;
					//float latch_prop = 0 ;
					for(int i=0; i<gateptr->event_list.size()-1;i++){
						if( gateptr->event_list[i]->qd > 0 ){
							for(int j=i+1;j<gateptr->event_list.size()-1;j++){
								if( gateptr->event_list[j]->qdn > 0 ){// we found a wave
									float no_tran_pre=1, no_tran_post=1;
									float elpp_a = 1;
									//for(int k=j+1; k<gateptr->event_list.size();k++){
									//	no_tran_post *= ( gateptr->event_list[k]->q0 + gateptr->event_list[k]->q1 );
									//}
									//for(int k=0; k<i;k++){
									//	no_tran_pre *= ( gateptr->event_list[k]->q0 + gateptr->event_list[k]->q1 );
									//}
									//elpp_a = no_tran_pre * gateptr->event_list[i]->qd * gateptr->event_list[j]->qdn *  no_tran_post ;
									elpp_a = gateptr->event_list[i]->qd ;
									float wave_duration =  gateptr->event_list[j+1]->time - gateptr->event_list[i+1]->time ;
									float lp_a = (float)(tsu+th+wave_duration)/(float)tperiod ;
									lp_a=(lp_a>1?1:lp_a);
									let_a = let_a * (1-elpp_a*lp_a);
									//let_a = let_a * (1-elpp_a);
									//let_a = let_a * (1-elpp_a*lp_a);
									//let_a += elpp_a*lp_a  ;
									//let_a = 1-let_a ;
									//latch_prop += elpp_a*lp_a ;
									//cout << endl << endl << endl ;
									//cout << "let_a= " << let_a  <<  "   elpp_a=" << elpp_a <<  "   lp_a=" << lp_a << "   -  " <<(1-elpp_a*lp_a) << endl ;;
									//cout << i << " " << j << ">> no_tran_pre=" << no_tran_pre << " no_tran_post=" << no_tran_post << "    elpp_a=" << elpp_a << endl ;
									//cout << gateptr->event_list[i]->qd << " " << gateptr->event_list[j]->qdn << endl ;
									//cout << " wave_duration=" << wave_duration << " _____ lp_a=" << lp_a << endl ;
									//cout << tsu << " "<< th << " "<< wave_duration<< " " << tperiod << endl ;
									//cout << " gateptr->event_list[i+1]->time=" << gateptr->event_list[i+1]->time << "    gateptr->event_list[j+1]->time=" << gateptr->event_list[j+1]->time << endl ; 
								}
							}
						}
						//let_a=1-let_a;
						if( gateptr->event_list[i]->qdn > 0  ){
							for(int j=i+1;j<gateptr->event_list.size()-1;j++){
								if( gateptr->event_list[j]->qd > 0 ){// we found a wave
									float no_tran_pre=1, no_tran_post=1;
									float elpp_an = 1;
									//for(int k=j+1; k<gateptr->event_list.size();k++){
									//	no_tran_post *= ( gateptr->event_list[k]->q0 + gateptr->event_list[k]->q1 );
									//}
									//for(int k=0; k<i;k++){
									//	no_tran_pre *= ( gateptr->event_list[k]->q0 + gateptr->event_list[k]->q1 );
									//}
									//elpp_an = no_tran_pre * gateptr->event_list[i]->qdn * gateptr->event_list[j]->qd *  no_tran_post ;
									elpp_an = gateptr->event_list[i]->qdn ;
									float wave_duration =  gateptr->event_list[j+1]->time - gateptr->event_list[i+1]->time ;
									float lp_an = (float)(tsu+th+wave_duration)/(float)tperiod ;
									lp_an=(lp_an>1?1:lp_an);
									let_an = let_an * (1-elpp_an*lp_an);
									//let_an = let_an * (1-elpp_an);
									//latch_prop += elpp_an*lp_an ;
									//let_an += elpp_an*lp_an ;
									//let_an = 1-let_an ;
									//cout << endl << endl << endl ;
									//cout << "let_an= " << let_an  <<  "   elpp_an=" << elpp_an <<  "   lp_an=" << lp_an << "   -  " <<(1-elpp_an*lp_an) << endl ;;
									//cout << i << " " << j << ">> no_tran_pre=" << no_tran_pre << " no_tran_post=" << no_tran_post << "    elpp_an=" << elpp_an << endl ;
									//cout << gateptr->event_list[i]->qdn << " " << gateptr->event_list[j]->qd << endl ;
									//cout << " wave_duration=" << wave_duration << " _____ lp_an=" << lp_an << endl ;
									//cout << tsu << " "<< th << " "<< wave_duration<< " " << tperiod << endl ;
									//cout << " gateptr->event_list[i+1]->time=" << gateptr->event_list[i+1]->time << "    gateptr->event_list[j+1]->time=" << gateptr->event_list[j+1]->time << endl ; 
								}
							}
						}
						

					}

					gptr1 = fadjlist_ptr[dfflist_ptr[ff_ind]->d];
					gptr2 = fadjlist_ptr[dfflist_ptr[ff_ind]->q];
					gptr2->p0 = gptr1->p0;
					gptr2->p1 = gptr1->p1;
					//gptr2->pd  = (let_a==0)? gptr1->pd :1-let_a ;
					//gptr2->pdn = (let_an==0)?gptr1->pdn : 1-let_an;
					gptr2->pd = 1-let_a ;//gptr1->pd;
					gptr2->pdn = 1-let_an ; //gptr1->pdn;

					cout << " * " << ff_ind << " " <<  gptr2->pd << " " << gptr2->pdn << endl ;

					//gptr2->pd = gptr1->pd;
					//gptr2->pdn = gptr1->pdn;


					//gptr2->pd = gptr1->pd;
					//gptr2->pdn = gptr1->pdn;

					//gptr2->pd = let_a ;//gptr1->pd;
					//gptr2->pdn = let_an ; //gptr1->pdn;

					//cout << ">>> Effect of Timing(a) : " << 1-let_a << " " << gptr1->pd << endl ;
					//cout << ">>> Effect of Timing(a'): " << 1-let_an << " " << gptr1->pdn << endl ;
					//cout << "*" << gateptr->event_list[i+1]->time << " | "<< gateptr->event_list[i+1]->time 
					//	<< " #p(0)= " << gateptr->event_list[i]->q0   << " #p(1)= " << gateptr->event_list[i]->q1  <<" #p(a)= " << gateptr->event_list[i]->qd << " - p(a')=" << gateptr->event_list[i]->qdn << endl ;


					//---------------------------------------------

					//tpsum = 0; tsum = 0;
					//for (k=0; k<gateptr->event_list.size()-1; k++){
					//	tk = gateptr->event_list[k+1]->time - gateptr->event_list[k]->time;
					//	tpsum += tk * (gateptr->event_list[k]->qd+gateptr->event_list[k]->qdn);
					//	cout << ">>>" << tpsum << " " << gateptr->event_list[k]->qd << " " << gateptr->event_list[k]->qdn << endl;
					//	tsum += tk;
					//}
					//if (tsum != 0) sys_failure *= 1- ((float)tpsum+(tsu+th)*(float)tpsum/(float)tsum)/(float)tperiod;
				}//for (ff_ind=0; ....
				*/

				/*
				// Algorithm4: heuristic approach
				// for each DFFs, we calcualte the LP * PP.
				for (int ff_ind=0; ff_ind<dff_num; ff_ind++){					
					gateptr = fadjlist_ptr[dfflist_ptr[ff_ind]->q];
					list_size = gateptr->event_list.size();
					float let_a=1, let_an=1;

					tpsum = 0;
					tsum = 0;
					float tsum_a = 0, tsum_an=0;
					float tpsum_a=0, tpsum_an=0;
					for (k=0; k<gateptr->event_list.size()-1; k++){
						tk = gateptr->event_list[k+1]->time - gateptr->event_list[k]->time;
						tpsum_a += tk * gateptr->event_list[k]->qd ;
						tpsum_an += tk * gateptr->event_list[k]->qdn ;
						//tpsum += tk * (gateptr->event_list[k]->pd+gateptr->event_list[k]->pdn);
						if(gateptr->event_list[k]->qd>0) tsum_a += tk ; 
						if(gateptr->event_list[k]->qdn>0) tsum_an+= tk ;
						tsum += tk;
					}
					//Babak:if( tsum != 0) let_a *= 1- ((float)tpsum_a*(tsum_a+tsu+th))/((float)tperiod*tsum);
					//Babak:if( tsum != 0) let_an *= 1- ((float)tpsum_an*(tsum_an+tsu+th))/((float)tperiod*tsum);
					if( tsum_a != 0) let_a *= 1- ((float)tpsum_a+(tsu+th)*(float)tpsum_a/(float)tsum_a)/(float)tperiod;
					if( tsum_an != 0) let_an *= 1- ((float)tpsum_an+(tsu+th)*(float)tpsum_an/(float)tsum_an)/(float)tperiod;
				//	if (tsum != 0){
				//		let_a *= 1- ((float)tpsum_a+(tsu+th)*(float)tpsum_a/(float)tsum_a)/(float)tperiod;
				//		let_an *= 1- ((float)tpsum_an+(tsu+th)*(float)tpsum_an/(float)tsum_an)/(float)tperiod;

						//let_a *= 1- ((float)tpsum_a+(tsu+th)*(float)tpsum_a/(float)tsum)/(float)tperiod;
						//let_an *= 1- ((float)tpsum_an+(tsu+th)*(float)tpsum_an/(float)tsum)/(float)tperiod;
						//sys_failure *= 1- ((float)tpsum+(tsu+th)*(float)tpsum/(float)tsum)/(float)tperiod;
				//	}

					gptr1 = fadjlist_ptr[dfflist_ptr[ff_ind]->d];
				 	gptr2 = fadjlist_ptr[dfflist_ptr[ff_ind]->q];
					gptr2->p0 = gptr1->p0;
					gptr2->p1 = gptr1->p1;
					//gptr2->pd  = (let_a==0)? gptr1->pd :1-let_a ;
					//gptr2->pdn = (let_an==0)?gptr1->pdn : 1-let_an;
					gptr2->pd = 1-let_a ;//gptr1->pd;
					gptr2->pdn = 1-let_an ; //gptr1->pdn;

					if(gptr2->pd<0)gptr2->pd=0;
					if(gptr2->pd>1)gptr2->pd=1;
					if(gptr2->pdn<0)gptr2->pdn=0;
					if(gptr2->pdn>1)gptr2->pdn=1;
				}//for (ff_ind=0; ....
				*/
				/*

				// Algorithm2: heuristic approach
				// for each DFFs, we calcualte the LP * PP.
				for (int ff_ind=0; ff_ind<dff_num; ff_ind++){					
				gateptr = fadjlist_ptr[dfflist_ptr[ff_ind]->q];
				list_size = gateptr->event_list.size();
				//cout << ">>>>>>>>>>>>>>>>>>>>>>>>..DFF= "<< ff_ind << " " << gateptr->event_list.size()-1<< endl ;
				float let_a=1, let_an=1;

				tpsum = 0;
				tsum = 0;
				float tsum_a = 0, tsum_an=0;
				float tpsum_a=0, tpsum_an=0;
				for (k=0; k<gateptr->event_list.size()-1; k++){
				tk = gateptr->event_list[k+1]->time - gateptr->event_list[k]->time;

				float x = 0 ; 
				bool found=false;
				for(int j=gateptr->event_list.size()-2;(j>k)&&(found==false);j--){
				if (gateptr->event_list[j]->qdn > 0 ){
				x=gateptr->event_list[j]->qdn;
				found=true;
				}
				}
				tpsum_a += tk * gateptr->event_list[k]->qd * x;
				found=false; x=0;
				for(int j=gateptr->event_list.size()-2;(j>k)&&(found==false);j--){
				if (gateptr->event_list[j]->qd > 0 ){
				x=gateptr->event_list[j]->qd;
				found=true;
				}
				}
				tpsum_an += tk * gateptr->event_list[k]->qdn *x ;
				//tpsum += tk * (gateptr->event_list[k]->pd+gateptr->event_list[k]->pdn);
				if(gateptr->event_list[k]->qd>0) tsum_a += tk ; 
				if(gateptr->event_list[k]->qdn>0) tsum_an+= tk ;
				tsum += tk;
				}
				if( tsum_a != 0) let_a *= 1- ((float)tpsum_a+(tsu+th)*(float)tpsum_a/(float)tsum_a)/(float)tperiod;
				if( tsum_an != 0) let_an *= 1- ((float)tpsum_an+(tsu+th)*(float)tpsum_an/(float)tsum_an)/(float)tperiod;
				//	if (tsum != 0){
				//		let_a *= 1- ((float)tpsum_a+(tsu+th)*(float)tpsum_a/(float)tsum_a)/(float)tperiod;
				//		let_an *= 1- ((float)tpsum_an+(tsu+th)*(float)tpsum_an/(float)tsum_an)/(float)tperiod;

				//let_a *= 1- ((float)tpsum_a+(tsu+th)*(float)tpsum_a/(float)tsum)/(float)tperiod;
				//let_an *= 1- ((float)tpsum_an+(tsu+th)*(float)tpsum_an/(float)tsum)/(float)tperiod;
				//sys_failure *= 1- ((float)tpsum+(tsu+th)*(float)tpsum/(float)tsum)/(float)tperiod;
				//	}

				gptr1 = fadjlist_ptr[dfflist_ptr[ff_ind]->d];
				gptr2 = fadjlist_ptr[dfflist_ptr[ff_ind]->q];
				gptr2->p0 = gptr1->p0;
				gptr2->p1 = gptr1->p1;
				//gptr2->pd  = (let_a==0)? gptr1->pd :1-let_a ;
				//gptr2->pdn = (let_an==0)?gptr1->pdn : 1-let_an;
				gptr2->pd = 1-let_a ;//gptr1->pd;
				gptr2->pdn = 1-let_an ; //gptr1->pdn;

				if(gptr2->pd<0)gptr2->pd=0;
				if(gptr2->pd>1)gptr2->pd=1;
				if(gptr2->pdn<0)gptr2->pdn=0;
				if(gptr2->pdn>1)gptr2->pdn=1;

				cout << " * " << ff_ind << " " <<  gptr2->pd << " " << gptr2->pdn << endl ;

				}//for (ff_ind=0; ....


				*/


/*

				//Algorithm 5: Babak@5'

				// for each DFFs, we calcualte the LP * PP.
				for (int ff_ind=0; ff_ind<dff_num; ff_ind++){					
					gateptr = fadjlist_ptr[dfflist_ptr[ff_ind]->q];
					list_size = gateptr->event_list.size();
					float let_a=1, let_an=1;

					tpsum = 0;
					tsum = 0;
					float tsum_a = 0, tsum_an=0;
					float tpsum_a=0, tpsum_an=0;
					vector<float> *Vda = new vector<float>();
					vector<float> *Vdan = new vector<float>();
					vector<float> *Vpa = new vector<float>();
					vector<float> *Vpan = new vector<float>();
					for(int i=0; i<gateptr->event_list.size()-1;i++){
						if( gateptr->event_list[i]->qd > 0 ){
							for(int j=i+1;j<gateptr->event_list.size()-1;j++){
								if( gateptr->event_list[j]->qdn > 0 ){// we found a wave
									float elpp_a = gateptr->event_list[i]->qd ;
									float wave_duration =  gateptr->event_list[j+1]->time - gateptr->event_list[i+1]->time ;
									float lp_a = (float)(tsu+th+wave_duration)/(float)tperiod ;
									Vda->push_back(wave_duration);
									Vpa->push_back(elpp_a);
									//lp_a=(lp_a>1?1:lp_a);
									//let_a = let_a * (1-elpp_a*lp_a);
								}
							}
						}
						if( gateptr->event_list[i]->qdn > 0  ){
							for(int j=i+1;j<gateptr->event_list.size()-1;j++){
								if( gateptr->event_list[j]->qd > 0 ){// we found a wave
									float elpp_an = elpp_an = gateptr->event_list[i]->qdn;
									float wave_duration =  gateptr->event_list[j+1]->time - gateptr->event_list[i+1]->time ;
									float lp_an = (float)(tsu+th+wave_duration)/(float)tperiod ;
									Vdan->push_back(wave_duration);
									Vpan->push_back(elpp_an);
									//lp_an=(lp_an>1?1:lp_an);
									//let_an = let_an * (1-elpp_an*lp_an);
								}
							}
						}
							
						float sum_a=0, sum_an=0;
						for(int i=0;i<Vda->size();i++){
							sum_a += (Vpa->at(i) * (float)(tsu+th+ Vda->at(i) ))/(float)tperiod;
							//sum  += Vda[i];
						}

						for(int i=0;i<Vdan->size();i++){
							sum_an += (Vpan->at(i) * (float)(tsu+th+Vdan->at(i)))/(float)tperiod;
							//sum  += Vda[i];
						}

						if( Vda->size() != 0 ) sum_a /= Vda->size();
						if( Vdan->size() != 0 ) sum_an /= Vdan->size();						
						let_a *= 1-sum_a;
						let_an *= 1-sum_an ;

						Vda->clear();
						Vdan->clear();
						Vpa->clear();
						Vpan->clear();
					}

					gptr1 = fadjlist_ptr[dfflist_ptr[ff_ind]->d];
					gptr2 = fadjlist_ptr[dfflist_ptr[ff_ind]->q];
					gptr2->p0 = gptr1->p0;
					gptr2->p1 = gptr1->p1;

					//cout << "@clk 1==>[previous value]\t" << gptr2->pd << "\t" << gptr2->pdn << endl;

					//gptr2->pd  = (let_a==0)? gptr1->pd :1-let_a ;
					//gptr2->pdn = (let_an==0)?gptr1->pdn : 1-let_an;
					gptr2->pd = 1-let_a ;//gptr1->pd;
					gptr2->pdn = 1-let_an ; //gptr1->pdn;


					if(gptr2->pd<0)gptr2->pd=0;
					if(gptr2->pd>1)gptr2->pd=1;
					if(gptr2->pdn<0)gptr2->pdn=0;
					if(gptr2->pdn>1)gptr2->pdn=1;

					//cout << "@clk 1==>[next-let] " << gptr2->pd << "\t" << gptr2->pdn << endl;
				}//for (ff_ind=0; ....
				*/
#endif