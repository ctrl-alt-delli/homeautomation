<template>
    <VContainer fluid align="left" justify="center">
        <!-- ROW 1 -->
        <v-row class="pd-1" style="max-width: 600px;" justify="center" align="center">
            <!-- COLUMN 1 -->
            <v-col class="col col1">
                <v-card class="pa-2 bg-background" flat height="250px" align="center">
                    <v-text-field class="mr-5" label="Start date" type="Date" density="compact" variant="solo" style="max-width: 300px;" v-model="start"></v-text-field>
                    <v-text-field class="mr-5" label="End date" type="Date" density="compact" variant="solo" style="max-width: 300px;" v-model="end"></v-text-field>
                    <v-spacer></v-spacer>
                    <v-btn class="text-caption rounded-0" text="Analyze" variant="elevated" @click="updateLineCharts();updateCards();"></v-btn>
                </v-card>
            </v-col>
            <!-- COLUMN 2 -->
            <v-col cols="4" align="center">
                <v-card title="Average" subtitle="For the selected period" width="400" height="250" variant="outlined" color="bg-surface" density="compact" rounded="lg">
                    <v-card-item align="center">
                        <span class="text-h1 text-primary font-weight-bold">
                            {{ avg.value }}
                            <span class="text-caption">Gal</span>
                        </span>
                    </v-card-item>
                </v-card>
            </v-col>
        </v-row>

        <!-- ROW 2 -->
        <v-row style="max-width: 1200px;">
            <!-- COLUMN 1 -->
            <v-col cols="12">
                <figure class="highcharts-figure">
                    <div id="container0"></div>
                </figure>
            </v-col>
            <!-- COLUMN 2 -->
            <v-col cols="12">
                <figure class="highcharts-figure">
                    <div id="container1"></div>
                </figure>
            </v-col>
        </v-row> 
    
    </VContainer>
</template>

<script setup>
/** JAVASCRIPT HERE */

// IMPORTS
import { ref,reactive,watch ,onMounted,onBeforeUnmount,computed } from "vue";  
import { useRoute ,useRouter } from "vue-router";

import { useAppStore } from "@/store/appStore";
import { useMqttStore } from "@/store/mqttStore"; // Import Mqtt Store
import { storeToRefs } from "pinia";

import Highcharts from 'highcharts';
import more from 'highcharts/highcharts-more';
import Exporting from 'highcharts/modules/exporting';
Exporting(Highcharts);
more(Highcharts);
 
 
// VARIABLES
const router      = useRouter();
const route       = useRoute();  
const AppStore    = useAppStore();
const Mqtt        = useMqttStore();
const { payload, payloadTopic } = storeToRefs(Mqtt);

const start       = ref(null);
const end         = ref(null);
const waterManagementChart = ref(null);
const heightWaterLevelChart = ref(null);
const reserve = reactive({ "avg": 0 })

const CreateCharts = async () => {
    // TEMPERATURE CHART
    waterManagementChart.value = Highcharts.chart('container1', {
        chart: { zoomType: 'x' },
        title: { text: 'Water Management Analysis', align: 'left' },
        yAxis: {
            title: { text: 'Water Reserve', style: { color: '#000000' } },
            labels: { format: '{value} Gal' }
        },
        xAxis: {
            type: 'datetime',
            title: { text: 'Time', style: { color: '#000000' } },
        },
        tooltip: { shared: true, },
        series: [
            {
                name: 'Reserve',
                type: 'spline',
                data: [],
                turboThreshold: 0,
                color: Highcharts.getOptions().colors[0]
            },
        ],
    });
    heightWaterLevelChart.value = Highcharts.chart('container2', {
        chart: { zoomType: 'x' },
        title: { text: 'Height and Water Level Correlation Analysis', align: 'left' },
        yAxis: {
            title: { text: 'Height', style: { color: '#000000' } },
            labels: { format: '{value} in' }
        },
        xAxis: {
            title: { text: 'Water Height', style: { color: '#000000' } },
            labels: { format: '{value} in' }
        },
        tooltip: { pointFormat: 'Water Height: {point.x} in <br/> Height: {point.y} in' },
        series: [
            {
                name: 'Analysis',
                type: 'scatter',
                data: [],
                turboThreshold: 0,
                color: Highcharts.getOptions().colors[0]
            },
        ],
    });
};

const updateLineCharts = async () => {
    if (!!start.value && !!end.value) {
        // Convert output from Textfield components to 10 digit timestamps
        let startDate = new Date(start.value).getTime() / 1000;
        let endDate = new Date(end.value).getTime() / 1000;
        // Fetch data from backend
        const data = await AppStore.retrieve_getAll(startDate, endDate);
        // Create arrays for each plot 
        let reserve = [];
        // Iterate through data variable and transform object to format recognized by highcharts
        data.forEach(row => {
            reserve.push({ "x": row.timestamp * 1000, "y": parseFloat(row.reserve.toFixed(0)) });
           
        });
        // Add data to  water Management Chart
        waterManagementChart.value.series[0].setData(reserve);
         }
}


const updateScatterPlot = async () => {
    if (!!start.value && !!end.value) {
        // Convert output from Textfield components to 10 digit timestamps
        let startDate = new Date(start.value).getTime() / 1000;
        let endDate = new Date(end.value).getTime() / 1000;
        // Fetch data from backend
        const data = await AppStore.retrieve_getAll(startDate, endDate);
        // Create arrays for each plot 
        let graph = [];
        

        // Iterate through data variable and transform object to format recognized by highcharts
        data.forEach(row => {
            graph.push({ "x": parseFloat(row.waterheight.toFixed(0)), "y": parseFloat(row.radar.toFixed(0)) });
           });
        
        ScatterPlot.value.series[0].setData(graph);
        

    }
}

const updateCard = async () => {
    // Retrieve Min, Max, Avg, Spread/Range
    if (!!start.value && !!end.value) {
        // 1. Convert start and end dates collected fron TextFields to 10 digit timestamps
        let startDate = new Date(start.value).getTime() / 1000;
        let endDate = new Date(end.value).getTime() / 1000;

        // 2. Fetch data from backend by calling the API functions
        const ReserveAVGfromDatabase = await AppStore.getaverage(startDate, endDate);
        console.log(ReserveAVGfromDatabase)

       
        reserve.avg = ReserveAVGfromDatabase[0].average.toFixed(1);
    }
}












// FUNCTIONS
onMounted(()=>{
    // THIS FUNCTION IS CALLED AFTER THIS COMPONENT HAS BEEN MOUNTED
   Mqtt.connect(); 
    setTimeout( ()=>{
        Mqtt.subscribe("620157584");
        Mqtt.subscribe("620157584_pub");
    },3000)
  CreateCharts(); 
});


onBeforeUnmount(()=>{
    // THIS FUNCTION IS CALLED RIGHT BEFORE THIS COMPONENT IS UNMOUNTED
    Mqtt.unsubcribeAll();
});


</script>


<style scoped>
/** CSS STYLE HERE */


</style>
  