

float Thermocouple_Ktype_VoltageToTempDegC(float voltage) {
   //http://srdata.nist.gov/its90/type_k/kcoefficients_inverse.html
   float coef_1[] = {0, 2.5173462e1, -1.1662878, -1.0833638, -8.9773540e-1};            // coefficients (in mV) for -200 to 0C, -5.891mv to 0mv
   float coef_2[] = {0, 2.508355e1, 7.860106e-2, -2.503131e-1, 8.315270e-2};            // coefficients (in mV) for 0 to 500C, 0mv to 20.644mv
   float coef_3[] = {-1.318058e2, 4.830222e1, -1.646031, 5.464731e-2, -9.650715e-4};    //whoa, that's hot...
   int i = 5;  // number of coefficients in array
   float temperature;

   float mVoltage = voltage *1e3;

   if(voltage < 0) {
    temperature = power_series(i, mVoltage, coef_1);
   }else if (voltage > 20.644){
    temperature = power_series(i, mVoltage, coef_3);
   }else{
    temperature = power_series(i, mVoltage, coef_2);
   }

   return(temperature);
}

float power_series(int n, float input, float coef[])
 {
      delay(10);
      float sum=0.0;
      int i;
      sum=coef[0];
      for(i=1;i<=(n-1);i++)
           sum=sum+(pow(input, (float)i)*coef[i]);
      return(sum);
 }

 
void Thermocouple_PrintResults(float referenceJuntionTemp, float relativeTemp, char units[]) {

  Serial.print("Thermocouple is measuring ");
  Serial.print(relativeTemp + referenceJuntionTemp);
  Serial.print(units);
  Serial.print(", which is calculated from relativeTemp ("); 
  Serial.print(relativeTemp);
  Serial.print(units);
  Serial.print(") + reference juction temp (");
  Serial.print(referenceJuntionTemp, 2);
  Serial.print(units);
  Serial.println(")\n\n");
}
