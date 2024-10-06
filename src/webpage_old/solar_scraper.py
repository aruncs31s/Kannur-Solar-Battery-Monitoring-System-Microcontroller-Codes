"""
- Author : Arun CS
- Date : 2024-10-03
- for v0.0.2
"""

# TODO: Implement function that returns 24 hours data with 24 points
import time
from datetime import datetime

import requests


class Scraper:
    def __init__(self, url):
        self.esp_url = url

    def get_data(self):
        try:
            esp_response = requests.get(self.esp_url)
            data_json = esp_response.json()
            data = [
                data_json["temperature"],
                data_json["humidity"],
                data_json["battery_voltage"],
                data_json["light_sensor_value"],
                data_json["led_relayState"],
                data_json["rain_volume"],
            ]
            return data
        except Exception as e:
            time.sleep(0.1)
            self.get_data()

    # def store_24_hours_data(self):
    # def get_24_hours_data(self):
    # TODO: Implement a function that stores the data in year -> Month -> Day -> Points
    # def store_data_json(self):
    #     self.data = self.get_data()
    #
    # def store_data_as_csv(self):
    #
