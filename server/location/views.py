from django.shortcuts import render
from django.http import JsonResponse, HttpResponse
from .models import Measurement, Device, Location, Calibration
from django.db.models import Avg
import threading


NB_MESURMENT_REQUIRED = 50
NB_ACCESS_POINTS = 3

lock = threading.Lock()

def calibrate(request, mac_address, x, y, z):
    w = Device.objects.filter(mac=mac_address)
    if w:
        w = w[0]
    else:
        w = Device(mac=mac_address,calibration=0)
        w.save()
        return JsonResponse({'code':'ACK'})
    
    mesurements = Measurement.objects.filter(dv_id=w)
    if len(mesurements) >= NB_MESURMENT_REQUIRED:
        l = Location.objects.filter(x=x,y=y,z=z)
        if l:
            l = l[0]
        else:
            l = Location(x=x, y=y, z=z)
            l.save()
        for i in range(1,NB_ACCESS_POINTS+1):
            ss_value = Measurement.ss_average(i, w)
            if ss_value == None:
                ss_value = -95
            c = Calibration(loc_id=l, ap_id=i, ss_value=ss_value)
            c.save()
        with lock:
            Measurement.objects.filter(dv_id=w).delete()
            w.delete()
        return JsonResponse({'code':'DONE'})
    else:
        return JsonResponse({'code':'WAIT'})
        


def check(request):
    return HttpResponse("\n"+"\n".join(Device.get_macs())+"\n")


def findme(request, mac_address):
    w = Device.objects.filter(mac=mac_address)
    if w:
        w = w[0]
    else:
        w = Device(mac=mac_address,calibration=0)
        w.save()
        return JsonResponse({'code':'ACK'})
    
    mesurements = Measurement.objects.filter(dv_id=w)
    if len(mesurements) >= NB_MESURMENT_REQUIRED:
        #calculate the position
        ss_values = {}
        for i in range(1,NB_ACCESS_POINTS+1):
            ss_values[i] = Measurement.objects.filter(dv_id=w, ap_id=i).aggregate(Avg('ss_value')).values()[0]
            if ss_values[i] == None:
                ss_values[i] = -95
        
        locations = Location.objects.all()
        min_distance = float("inf")
        min_reference = None
        for location in locations:
            calibrations = Calibration.objects.filter(loc_id=location)
            distance = 0
            for i in range(1,NB_ACCESS_POINTS+1):
                distance += pow(calibrations.filter(ap_id=i)[0].ss_value - ss_values[i],2)
            if distance < min_distance:
                min_distance = distance
                min_location = location
        
        preload = {'code':'MAP', 'x':min_location.x,'y':min_location.y,'z':min_location.z}
        
        #Clean the database
        with lock:
            Measurement.objects.filter(dv_id=w).delete()
            w.delete()
        return JsonResponse(preload)
    else:
        return JsonResponse({'code':'WAIT'})


def gotcha(request, ap_id, mac_address, RSSI):
    with lock:
        w = Device.objects.filter(mac=mac_address)
        if w:
            m = Measurement(ap_id=ap_id, dv_id=w[0], ss_value=RSSI)
            m.save()
    return JsonResponse({'code':0})



def reset(request):
    with lock:
        Measurement.objects.all().delete()
        Calibration.objects.all().delete()
        Location.objects.all().delete()
        Device.objects.all().delete()
    return JsonResponse({'code':0})




