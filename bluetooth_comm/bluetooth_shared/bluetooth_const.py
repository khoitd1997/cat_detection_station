# store bluetooth constants for both client and station
data_separator = b"||"  # used for separating bytes array in socket data
max_data_len = 10  # how many bytes to express len of package

cat_station_addr = "B8:27:EB:63:F0:19"
host_addr = "00:1A:7D:DA:71:13"

weather_service_uuid = "d7945c1a-955d-4d3f-989c-e74ac51cf291"
weather_service_name = "Weather Service"

cat_info_size_len = 10  # 10 character to express how long the dict will be
cat_alert_service_uuid = "23a77f17-1a9b-4779-957b-77cab8eefb6e"
cat_alert_service_name = "Cat Alert Service"
cat_picture_size = 30000

logger_format = '%(asctime)-15s %(message)s'

# sample message for the weather service
weather_info_prototype = {
    "temperature": 1.1,  # celsius
    "pressure": 1.1,  # Pascal
    "humidity": 50.5,  # Percent
}

cat_info_prototype = {
    "catIsHere": True,
    "timestamp": "Tue Jan 13 10:17:09 2009",
}