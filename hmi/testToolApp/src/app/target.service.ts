import { Injectable } from '@angular/core';
import { HttpClient } from '@angular/common/http';
import { Target } from './target';
import { Observable, of } from 'rxjs';
import { environment } from '../environments/environment.prod';

@Injectable({
  providedIn: 'root'
})
export class TargetService {

  constructor(private http: HttpClient) { }

  private baseUrl = environment.baseURL + 'api/targets';

  getTargets(): Observable<Target[]> {
    const targets = this.http.get<Target[]>(this.baseUrl);
    return targets;
  }
}
