#ifndef __DHT_HPP__
#define __DHT_HPP__

namespace LDM {
class DHT {
public:
	DHT() : temperature(0), humidity(0) {};

	float getHumidity() {
		return this->humidity;
	}

	float getTemperature() {
		return this->temperature;
	}

	void setHumidity(float humidity) {
		this->humidity = humidity;
	}

	void setTemperature(float temperature) {
		this->temperature = temperature;
	}

private:
	float temperature;
	float humidity;
};
}
#endif
