#ifndef __BME680_HPP__
#define __BME680_HPP__

namespace LDM {
class BME680 {
public:
	BME680() : temperature(0), humidity(0), pressure(0), gas(0) {};

	float getHumidity() {
		return this->humidity;
	}

	float getTemperature() {
		return this->temperature;
	}

	float getPressure() {
		return this->pressure;
	}

	float getGas() {
		return this->gas;
	}

	void setHumidity(float humidity) {
		this->humidity = humidity;
	}

	void setTemperature(float temperature) {
		this->temperature = temperature;
	}

   void setPressure(float pressure) {
		this->pressure = pressure;
	}

	void setGas(float gas) {
		this->gas = gas;
	}

private:
	float temperature;
	float humidity;
   int pressure;
   int gas;
};
}
#endif